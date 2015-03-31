cmake_minimum_required(VERSION 2.8)

PROJECT(FighterPilot CXX)
SET(CMAKE_CXX_FLAGS "-std=c++1y -Wall -O0 -ggdb -fsanitize=address")
#Debug:"-O0 -g"  Release:"-02"
#-fsanitize=address -fsanitize=undefined -fno-optimize-sibling-calls

add_definitions(-D_DEBUG)

#set_source_files_properties(glew.c PROPERTIES LANGUAGE CXX)

ADD_EXECUTABLE(FighterPilot xml/tinyxml2.cpp aaGun.cpp angle.cpp assetLoader.cpp blackSmoke.cpp bomb.cpp bullet.cpp bulletEffect.cpp campaign.cpp collide.cpp contrail.cpp contrailSmall.cpp controlManager.cpp dataManager.cpp debrisSmokeTrail.cpp debugBreak.cpp definitions.cpp dogFight.cpp engine.cpp ephemeris.cpp explosionFlash.cpp explosionFlash2.cpp explosionSmoke.cpp explosionSparks.cpp fileManager.cpp fireball.cpp game.cpp graphicsManager.cpp groundExplosionFlash.cpp input.cpp level.cpp levelEditor.cpp menus.cpp menuScreen.cpp missile.cpp object.cpp objectInfo.cpp objectList.cpp openGLgraphics.cpp particleExplosion.cpp particleManager.cpp plane.cpp planeContrail.cpp planeEngines.cpp player.cpp random.cpp sceneManager.cpp settings.cpp ship.cpp smokeTrail.cpp soundManager.cpp splash.cpp splash2.cpp splitScreen.cpp terrain.cpp time.cpp tracer.cpp worldManager.cpp)
TARGET_LINK_LIBRARIES(FighterPilot X11 GL GLU GLEW Xxf86vm png z pthread)

add_custom_target(run ./FighterPilot DEPENDS FighterPilot)