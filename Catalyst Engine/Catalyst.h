#pragma once

/*CORE*/
#include "include/Utilities.h"
#include "include/InputManager.h"

/*GRAPHICS*/

//-->/*VIEW AND MOVEMENTS*/
#include "include/ViewportManager.h"
#include "include/Transform.h"
#include "include/Camera.h"
#include "include/Texture.h"

//-->/*INTERFACES*/
#include "include/IActor.h"
#include "include/IComponent.h"
#include "include/IApplication.h"
#include "include/IRenderingEngine.h"

//-->/*SHADERS*/
#include "include/Shader.h"
#include "include/BasicShader.h"
#include "include/SineShader.h"
#include "include/FogShader.h"
#include "include/DirectionalShader.h"
#include "include/Lightmap Shader.h"

//-->/*MESH*/
#include "include/Mesh.h"
#include "include/MeshFactory.h"

//-->/*ACTOR-COMPONENT*/
#include "include/Actor.h"

//-->/*SCENE MANAGER*/
#include "include/SceneManager.h"

//-->/*RENDERERS*/
#include "include/BasicRenderer.h"
#include "include\LightsRenderer.h"
#include "include\SpriteRenderer.h"

//-->/*MISC*/
#include "include\ResourceCache.h"
#include "include\IntersectionTest.h"
#include "include\FPSCamera.h"	
#include "include\Animator.h"	

//-->/*PHYSICS*/
//#include "include\PhysicsEngine.h"