// Fill out your copyright notice in the Description page of Project Settings.


#include "FindRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "blackboard_keys.h"
#include "AIController.h"

UFindRandomLocation::UFindRandomLocation(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UFindRandomLocation::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// get AI controller and its NPC
	AAIController* cont = Cast<AAIController>(owner_comp.GetAIOwner());
	auto const npc = cont -> GetPawn();
	AEnemy* enemy = Cast<AEnemy>(cont);

	// obtain npc location to use as an origin location
	FVector const origin = npc->GetActorLocation();

	// get the navigation system and generate a random location on the NavMesh
	UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation loc;
	if (nav_sys->GetRandomPointInNavigableRadius(origin, search_radius, loc, nullptr))
	{

		enemy->get_blackboard()->SetValueAsVector(GetSelectedBlackboardKey(), loc.Location);
	}

	// finish with success
	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
