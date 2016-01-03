cmake_minimum_required(VERSION 3.0)

PROJECT(FighterPilot CXX)
#SET(CMAKE_CXX_COMPILER "clang")
SET(CMAKE_CXX_FLAGS "-std=c++1y -Wall -O1 -ggdb -Winvalid-pch -Wno-sign-compare")
#Debug:"-O0 -g"  Release:"-02"
#-fsanitize=address -fsanitize=undefined -fno-optimize-sibling-calls
# -ftime-report -H
add_definitions(-D_DEBUG)

#set_source_files_properties(glew.c PROPERTIES LANGUAGE CXX)

# Somewhat hacky solution to get cmake to work with a precompiled header...
get_property(defines DIRECTORY PROPERTY COMPILE_DEFINITIONS)
separate_arguments(flags UNIX_COMMAND "${CMAKE_CXX_FLAGS}")
add_custom_command(OUTPUT engine.h.gch DEPENDS engine.h *.h COMMAND
  ${CMAKE_CXX_COMPILER} -c engine.h -o engine.h.gch ${flags} -D${defines}
  COMMENT "Building precompiled header"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(generateheader ALL DEPENDS engine.h.gch)

ADD_EXECUTABLE(FighterPilot xml/tinyxml2.cpp aaGun.cpp angle.cpp assetLoader.cpp blackSmoke.cpp bomb.cpp bullet.cpp bulletEffect.cpp campaign.cpp collide.cpp contrail.cpp contrailSmall.cpp controlManager.cpp dataManager.cpp debrisSmokeTrail.cpp debugBreak.cpp definitions.cpp dogFight.cpp engine.cpp ephemeris.cpp explosionFlash.cpp explosionFlash2.cpp explosionSmoke.cpp explosionSparks.cpp fileManager.cpp fireball.cpp game.cpp graphicsManager.cpp groundExplosionFlash.cpp input.cpp level.cpp menus.cpp menuScreen.cpp missile.cpp object.cpp objectInfo.cpp objectList.cpp openGLgraphics.cpp particleExplosion.cpp particleManager.cpp plane.cpp planeContrail.cpp planeEngines.cpp player.cpp random.cpp sceneManager.cpp settings.cpp ship.cpp smokeTrail.cpp soundManager.cpp splash.cpp splash2.cpp splitScreen.cpp terrain.cpp time.cpp tracer.cpp worldManager.cpp)
TARGET_LINK_LIBRARIES(FighterPilot X11 GL GLU Xxf86vm png z pthread GLEW)
add_dependencies(FighterPilot generateheader)

add_custom_target(run ./FighterPilot DEPENDS FighterPilot)
