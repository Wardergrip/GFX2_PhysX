#include "stdafx.h"
#include "SceneSelector.h"
#include "SceneManager.h"

//Change this define to activate/deactive the corresponding scenes
// W01 - W02  (#define ...)

#define W01
//#define W02

#ifdef W01
#include "TestScene.h"
#include "PhysXTestScene.h"
#include "BouncingBallsScene.h"
#include "FrictionTestScene.h"
#include "BoxForceScene.h"
#include "W1_AssignmentScene.h"
#endif

#ifdef W02
#include "PhysxMeshTestScene.h"
#include "TriggerTestScene.h"
#include "AudioTestScene.h"
#include "W2_AssignmentScene.h"
#endif

namespace dae
{
	void AddScenes(SceneManager * pSceneManager)
	{

#ifdef W01
		//pSceneManager->AddGameScene(new TestScene());
		//pSceneManager->AddGameScene(new PhysXTestScene());
		//pSceneManager->AddGameScene(new BouncingBallsScene());
		//pSceneManager->AddGameScene(new FrictionTestScene());
		//pSceneManager->AddGameScene(new BoxForceScene());
		pSceneManager->AddGameScene(new W1_AssignmentScene());
#endif

#ifdef W02
		//pSceneManager->AddGameScene(new PhysxMeshTestScene());
		//pSceneManager->AddGameScene(new TriggerTestScene());
		//pSceneManager->AddGameScene(new AudioTestScene());
		pSceneManager->AddGameScene(new W2_AssignmentScene());
#endif

	}
}

