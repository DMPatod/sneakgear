#include "CodexBlueprintMCP.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "EditorAssetLibrary.h"
#include "Engine/Blueprint.h"
#include "GameFramework/Actor.h"
#include "HttpServerModule.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "IHttpRouter.h"
#include "JsonObjectConverter.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#define LOCTEXT_NAMESPACE "FCodexBlueprintMCPModule"

namespace CodexBlueprintMCP
{
constexpr uint32 ServerPort = 8765;

TSharedPtr<IHttpRouter> Router;
TArray<FHttpRouteHandle> RouteHandles;

FString ToJson(const TSharedRef<FJsonObject>& Object)
{
	FString Result;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Result);
	FJsonSerializer::Serialize(Object, Writer);
	return Result;
}

bool SendJson(const FHttpResultCallback& OnComplete, const TSharedRef<FJsonObject>& Object, const EHttpServerResponseCodes Code = EHttpServerResponseCodes::Ok)
{
	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(ToJson(Object), TEXT("application/json"));
	Response->Code = Code;
	OnComplete(MoveTemp(Response));
	return true;
}

bool SendError(const FHttpResultCallback& OnComplete, const FString& Message, const EHttpServerResponseCodes Code = EHttpServerResponseCodes::BadRequest)
{
	const TSharedRef<FJsonObject> Object = MakeShared<FJsonObject>();
	Object->SetBoolField(TEXT("ok"), false);
	Object->SetStringField(TEXT("error"), Message);
	return SendJson(OnComplete, Object, Code);
}

TSharedPtr<FJsonObject> ParseBody(const FHttpServerRequest& Request)
{
	const FUTF8ToTCHAR ConvertedBody(reinterpret_cast<const ANSICHAR*>(Request.Body.GetData()), Request.Body.Num());
	const FString Body(ConvertedBody.Length(), ConvertedBody.Get());
	TSharedPtr<FJsonObject> Object;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
	if (!FJsonSerializer::Deserialize(Reader, Object))
	{
		return nullptr;
	}
	return Object;
}

bool ReadStringField(const TSharedPtr<FJsonObject>& Object, const TCHAR* FieldName, FString& OutValue)
{
	return Object.IsValid() && Object->TryGetStringField(FieldName, OutValue) && !OutValue.IsEmpty();
}

UClass* ResolveParentClass(const FString& ParentClass)
{
	if (ParentClass.IsEmpty() || ParentClass.Equals(TEXT("Actor"), ESearchCase::IgnoreCase))
	{
		return AActor::StaticClass();
	}

	if (UClass* ExistingClass = FindObject<UClass>(nullptr, *ParentClass))
	{
		return ExistingClass;
	}

	return LoadObject<UClass>(nullptr, *ParentClass);
}

UEdGraph* FindGraphByName(UBlueprint* Blueprint, const FString& GraphName)
{
	if (!Blueprint)
	{
		return nullptr;
	}

	TArray<UEdGraph*> Graphs;
	Blueprint->GetAllGraphs(Graphs);
	for (UEdGraph* Graph : Graphs)
	{
		if (Graph && Graph->GetName().Equals(GraphName, ESearchCase::IgnoreCase))
		{
			return Graph;
		}
	}
	return nullptr;
}

TSharedRef<FJsonObject> DescribePin(const UEdGraphPin* Pin)
{
	const TSharedRef<FJsonObject> PinObject = MakeShared<FJsonObject>();
	if (!Pin)
	{
		return PinObject;
	}

	PinObject->SetStringField(TEXT("name"), Pin->PinName.ToString());
	PinObject->SetStringField(TEXT("direction"), Pin->Direction == EGPD_Input ? TEXT("input") : TEXT("output"));
	PinObject->SetStringField(TEXT("category"), Pin->PinType.PinCategory.ToString());
	PinObject->SetBoolField(TEXT("linked"), Pin->LinkedTo.Num() > 0);
	return PinObject;
}

TSharedRef<FJsonObject> DescribeNode(const UEdGraphNode* Node)
{
	const TSharedRef<FJsonObject> NodeObject = MakeShared<FJsonObject>();
	if (!Node)
	{
		return NodeObject;
	}

	NodeObject->SetStringField(TEXT("name"), Node->GetName());
	NodeObject->SetStringField(TEXT("title"), Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString());
	NodeObject->SetStringField(TEXT("class"), Node->GetClass()->GetName());

	TArray<TSharedPtr<FJsonValue>> Pins;
	for (const UEdGraphPin* Pin : Node->Pins)
	{
		Pins.Add(MakeShared<FJsonValueObject>(DescribePin(Pin)));
	}
	NodeObject->SetArrayField(TEXT("pins"), Pins);
	return NodeObject;
}

TSharedRef<FJsonObject> DescribeBlueprint(UBlueprint* Blueprint)
{
	const TSharedRef<FJsonObject> Object = MakeShared<FJsonObject>();
	Object->SetBoolField(TEXT("ok"), true);

	if (!Blueprint)
	{
		Object->SetBoolField(TEXT("ok"), false);
		Object->SetStringField(TEXT("error"), TEXT("Blueprint could not be loaded."));
		return Object;
	}

	Object->SetStringField(TEXT("name"), Blueprint->GetName());
	Object->SetStringField(TEXT("path"), Blueprint->GetPathName());
	Object->SetStringField(TEXT("parent_class"), Blueprint->ParentClass ? Blueprint->ParentClass->GetPathName() : FString());
	Object->SetStringField(TEXT("generated_class"), Blueprint->GeneratedClass ? Blueprint->GeneratedClass->GetPathName() : FString());

	TArray<TSharedPtr<FJsonValue>> Variables;
	for (const FBPVariableDescription& Variable : Blueprint->NewVariables)
	{
		const TSharedRef<FJsonObject> VariableObject = MakeShared<FJsonObject>();
		VariableObject->SetStringField(TEXT("name"), Variable.VarName.ToString());
		VariableObject->SetStringField(TEXT("category"), Variable.VarType.PinCategory.ToString());
		VariableObject->SetStringField(TEXT("sub_category"), Variable.VarType.PinSubCategory.ToString());
		Variables.Add(MakeShared<FJsonValueObject>(VariableObject));
	}
	Object->SetArrayField(TEXT("variables"), Variables);

	TArray<TSharedPtr<FJsonValue>> GraphsJson;
	TArray<UEdGraph*> Graphs;
	Blueprint->GetAllGraphs(Graphs);
	for (const UEdGraph* Graph : Graphs)
	{
		const TSharedRef<FJsonObject> GraphObject = MakeShared<FJsonObject>();
		GraphObject->SetStringField(TEXT("name"), Graph ? Graph->GetName() : FString());

		TArray<TSharedPtr<FJsonValue>> Nodes;
		if (Graph)
		{
			for (const UEdGraphNode* Node : Graph->Nodes)
			{
				Nodes.Add(MakeShared<FJsonValueObject>(DescribeNode(Node)));
			}
		}
		GraphObject->SetArrayField(TEXT("nodes"), Nodes);
		GraphsJson.Add(MakeShared<FJsonValueObject>(GraphObject));
	}
	Object->SetArrayField(TEXT("graphs"), GraphsJson);

	return Object;
}

UBlueprint* LoadBlueprintAsset(const FString& AssetPath)
{
	UObject* Asset = UEditorAssetLibrary::LoadAsset(AssetPath);
	if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
	{
		return Blueprint;
	}
	return LoadObject<UBlueprint>(nullptr, *AssetPath);
}

FEdGraphPinType MakePinType(const FString& TypeName)
{
	FEdGraphPinType PinType;
	const FString NormalizedType = TypeName.ToLower();

	if (NormalizedType == TEXT("bool") || NormalizedType == TEXT("boolean"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	}
	else if (NormalizedType == TEXT("int") || NormalizedType == TEXT("integer"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
	}
	else if (NormalizedType == TEXT("float") || NormalizedType == TEXT("double") || NormalizedType == TEXT("real"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Real;
		PinType.PinSubCategory = UEdGraphSchema_K2::PC_Double;
	}
	else if (NormalizedType == TEXT("name"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Name;
	}
	else if (NormalizedType == TEXT("text"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Text;
	}
	else if (NormalizedType == TEXT("vector"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		PinType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
	}
	else
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_String;
	}

	return PinType;
}

bool HandleHealth(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	const TSharedRef<FJsonObject> Object = MakeShared<FJsonObject>();
	Object->SetBoolField(TEXT("ok"), true);
	Object->SetStringField(TEXT("service"), TEXT("CodexBlueprintMCP"));
	Object->SetNumberField(TEXT("port"), ServerPort);
	return SendJson(OnComplete, Object);
}

bool HandleListBlueprints(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	FARFilter Filter;
	Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add(TEXT("/Game"));

	TArray<FAssetData> Assets;
	FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get().GetAssets(Filter, Assets);

	TArray<TSharedPtr<FJsonValue>> Blueprints;
	for (const FAssetData& Asset : Assets)
	{
		const TSharedRef<FJsonObject> AssetObject = MakeShared<FJsonObject>();
		AssetObject->SetStringField(TEXT("name"), Asset.AssetName.ToString());
		AssetObject->SetStringField(TEXT("path"), Asset.GetSoftObjectPath().ToString());
		AssetObject->SetStringField(TEXT("package"), Asset.PackageName.ToString());
		Blueprints.Add(MakeShared<FJsonValueObject>(AssetObject));
	}

	const TSharedRef<FJsonObject> Object = MakeShared<FJsonObject>();
	Object->SetBoolField(TEXT("ok"), true);
	Object->SetArrayField(TEXT("blueprints"), Blueprints);
	return SendJson(OnComplete, Object);
}

bool HandleReadBlueprint(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	const TSharedPtr<FJsonObject> Body = ParseBody(Request);
	FString AssetPath;
	if (!ReadStringField(Body, TEXT("asset_path"), AssetPath))
	{
		return SendError(OnComplete, TEXT("Missing required string field: asset_path"));
	}

	return SendJson(OnComplete, DescribeBlueprint(LoadBlueprintAsset(AssetPath)));
}

bool HandleCreateBlueprint(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	const TSharedPtr<FJsonObject> Body = ParseBody(Request);
	FString PackagePath;
	FString Name;
	if (!ReadStringField(Body, TEXT("package_path"), PackagePath))
	{
		return SendError(OnComplete, TEXT("Missing required string field: package_path"));
	}
	if (!ReadStringField(Body, TEXT("name"), Name))
	{
		return SendError(OnComplete, TEXT("Missing required string field: name"));
	}

	FString ParentClassPath;
	Body->TryGetStringField(TEXT("parent_class"), ParentClassPath);
	UClass* ParentClass = ResolveParentClass(ParentClassPath);
	if (!ParentClass)
	{
		return SendError(OnComplete, FString::Printf(TEXT("Could not resolve parent class: %s"), *ParentClassPath));
	}

	const FString PackageName = PackagePath / Name;
	if (!FPackageName::IsValidLongPackageName(PackageName))
	{
		return SendError(OnComplete, FString::Printf(TEXT("Invalid package name: %s"), *PackageName));
	}

	UPackage* Package = CreatePackage(*PackageName);
	UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(
		ParentClass,
		Package,
		FName(*Name),
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass(),
		FName(TEXT("CodexBlueprintMCP")));

	FAssetRegistryModule::AssetCreated(Blueprint);
	Package->MarkPackageDirty();
	UEditorAssetLibrary::SaveLoadedAsset(Blueprint);

	return SendJson(OnComplete, DescribeBlueprint(Blueprint));
}

bool HandleAddVariable(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	const TSharedPtr<FJsonObject> Body = ParseBody(Request);
	FString AssetPath;
	FString VariableName;
	if (!ReadStringField(Body, TEXT("asset_path"), AssetPath))
	{
		return SendError(OnComplete, TEXT("Missing required string field: asset_path"));
	}
	if (!ReadStringField(Body, TEXT("name"), VariableName))
	{
		return SendError(OnComplete, TEXT("Missing required string field: name"));
	}

	FString TypeName = TEXT("string");
	Body->TryGetStringField(TEXT("type"), TypeName);

	UBlueprint* Blueprint = LoadBlueprintAsset(AssetPath);
	if (!Blueprint)
	{
		return SendError(OnComplete, FString::Printf(TEXT("Could not load Blueprint: %s"), *AssetPath));
	}

	if (!FBlueprintEditorUtils::AddMemberVariable(Blueprint, FName(*VariableName), MakePinType(TypeName)))
	{
		return SendError(OnComplete, FString::Printf(TEXT("Could not add variable: %s"), *VariableName));
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	FKismetEditorUtilities::CompileBlueprint(Blueprint);
	UEditorAssetLibrary::SaveLoadedAsset(Blueprint);
	return SendJson(OnComplete, DescribeBlueprint(Blueprint));
}

bool HandleCompileBlueprint(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	const TSharedPtr<FJsonObject> Body = ParseBody(Request);
	FString AssetPath;
	if (!ReadStringField(Body, TEXT("asset_path"), AssetPath))
	{
		return SendError(OnComplete, TEXT("Missing required string field: asset_path"));
	}

	UBlueprint* Blueprint = LoadBlueprintAsset(AssetPath);
	if (!Blueprint)
	{
		return SendError(OnComplete, FString::Printf(TEXT("Could not load Blueprint: %s"), *AssetPath));
	}

	FKismetEditorUtilities::CompileBlueprint(Blueprint);
	UEditorAssetLibrary::SaveLoadedAsset(Blueprint);

	const TSharedRef<FJsonObject> Object = MakeShared<FJsonObject>();
	Object->SetBoolField(TEXT("ok"), true);
	Object->SetStringField(TEXT("asset_path"), Blueprint->GetPathName());
	return SendJson(OnComplete, Object);
}

void BindRoute(const TCHAR* Path, const EHttpServerRequestVerbs Verb, const FHttpRequestHandler& Handler)
{
	if (Router.IsValid())
	{
		RouteHandles.Add(Router->BindRoute(FHttpPath(Path), Verb, Handler));
	}
}
}

void FCodexBlueprintMCPModule::StartupModule()
{
	StartHttpServer();
}

void FCodexBlueprintMCPModule::ShutdownModule()
{
	StopHttpServer();
}

void FCodexBlueprintMCPModule::StartHttpServer()
{
	using namespace CodexBlueprintMCP;

	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	Router = HttpServerModule.GetHttpRouter(ServerPort);
	if (!Router.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("CodexBlueprintMCP could not create HTTP router on port %d."), ServerPort);
		return;
	}

	BindRoute(TEXT("/codex-blueprint/health"), EHttpServerRequestVerbs::VERB_GET, FHttpRequestHandler::CreateStatic(&HandleHealth));
	BindRoute(TEXT("/codex-blueprint/list"), EHttpServerRequestVerbs::VERB_POST, FHttpRequestHandler::CreateStatic(&HandleListBlueprints));
	BindRoute(TEXT("/codex-blueprint/read"), EHttpServerRequestVerbs::VERB_POST, FHttpRequestHandler::CreateStatic(&HandleReadBlueprint));
	BindRoute(TEXT("/codex-blueprint/create"), EHttpServerRequestVerbs::VERB_POST, FHttpRequestHandler::CreateStatic(&HandleCreateBlueprint));
	BindRoute(TEXT("/codex-blueprint/add-variable"), EHttpServerRequestVerbs::VERB_POST, FHttpRequestHandler::CreateStatic(&HandleAddVariable));
	BindRoute(TEXT("/codex-blueprint/compile"), EHttpServerRequestVerbs::VERB_POST, FHttpRequestHandler::CreateStatic(&HandleCompileBlueprint));

	HttpServerModule.StartAllListeners();
	UE_LOG(LogTemp, Display, TEXT("CodexBlueprintMCP listening on http://127.0.0.1:%d/codex-blueprint."), ServerPort);
}

void FCodexBlueprintMCPModule::StopHttpServer()
{
	using namespace CodexBlueprintMCP;

	if (Router.IsValid())
	{
		for (const FHttpRouteHandle& RouteHandle : RouteHandles)
		{
			Router->UnbindRoute(RouteHandle);
		}
		RouteHandles.Reset();
	}

	Router.Reset();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCodexBlueprintMCPModule, CodexBlueprintMCP)
