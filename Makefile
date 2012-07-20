# Compiler flags...
CPP_COMPILER = g++ -std=c++0x
C_COMPILER = gcc

# Include paths...
Debug_Include_Path=
Release_Include_Path=

# Library paths...
Debug_Library_Path=
Release_Library_Path=

# Additional libraries...
Debug_Libraries= -lGL -lGLU -lX11 -lXxf86vm -lpng -lz
Release_Libraries= -lGL -lGLU -lX11 -lXxf86vm -lpng -lz

# Preprocessor definitions...
Debug_Preprocessor_Definitions=-D GCC_BUILD -D _DEBUG 
Release_Preprocessor_Definitions=-D GCC_BUILD -D NDEBUG 

# Implictly linked object files...
Debug_Implicitly_Linked_Objects=
Release_Implicitly_Linked_Objects=

# Compiler flags...
Debug_Compiler_Flags=-O0 -g 
Release_Compiler_Flags=-O2 

# Builds all configurations for this project...
.PHONY: build_all_configurations
build_all_configurations: Debug Release 

# Builds the Debug configuration...
.PHONY: Debug
Debug: create_folders gccDebug/game.o gccDebug/campaign.o gccDebug/levelEditor.o gccDebug/menus.o gccDebug/dogFight.o gccDebug/splitScreen.o gccDebug/blackSmoke.o gccDebug/bulletEffect.o gccDebug/contrail.o gccDebug/contrailSmall.o gccDebug/debrisSmokeTrail.o gccDebug/explosionFlash.o gccDebug/explosionFlash2.o gccDebug/explosionSmoke.o gccDebug/explosionSparks.o gccDebug/groundExposionFlash.o gccDebug/particleExplosion.o gccDebug/planeContrail.o gccDebug/planeEngines.o gccDebug/smokeTrail.o gccDebug/splash.o gccDebug/splash2.o gccDebug/plane.o gccDebug/aaGun.o gccDebug/bomb.o gccDebug/missile.o gccDebug/bullet.o gccDebug/player.o gccDebug/settings.o gccDebug/level.o gccDebug/objectInfo.o gccDebug/debugBreak.o gccDebug/engine.o gccDebug/graphicsManager.o gccDebug/openGLgraphics.o gccDebug/collide.o gccDebug/gameMath.o gccDebug/angle.o gccDebug/dataManager.o gccDebug/assetLoader.o gccDebug/object.o gccDebug/objectList.o gccDebug/menuScreen.o gccDebug/particleManager.o gccDebug/random.o gccDebug/time.o gccDebug/input.o gccDebug/script.o gccDebug/worldManager.o gccDebug/terrain.o gccDebug/fileManager.o gccDebug/sceneManager.o gccDebug/controlManager.o gccDebug/definitions.o gccDebug/ephemeris.o gccDebug/glew.o 
	g++ gccDebug/game.o gccDebug/campaign.o gccDebug/levelEditor.o gccDebug/menus.o gccDebug/dogFight.o gccDebug/splitScreen.o gccDebug/blackSmoke.o gccDebug/bulletEffect.o gccDebug/contrail.o gccDebug/contrailSmall.o gccDebug/debrisSmokeTrail.o gccDebug/explosionFlash.o gccDebug/explosionFlash2.o gccDebug/explosionSmoke.o gccDebug/explosionSparks.o gccDebug/groundExposionFlash.o gccDebug/particleExplosion.o gccDebug/planeContrail.o gccDebug/planeEngines.o gccDebug/smokeTrail.o gccDebug/splash.o gccDebug/splash2.o gccDebug/plane.o gccDebug/aaGun.o gccDebug/bomb.o gccDebug/missile.o gccDebug/bullet.o gccDebug/player.o gccDebug/settings.o gccDebug/level.o gccDebug/objectInfo.o gccDebug/debugBreak.o gccDebug/engine.o gccDebug/graphicsManager.o gccDebug/openGLgraphics.o gccDebug/collide.o gccDebug/gameMath.o gccDebug/angle.o gccDebug/dataManager.o gccDebug/assetLoader.o gccDebug/object.o gccDebug/objectList.o gccDebug/menuScreen.o gccDebug/particleManager.o gccDebug/random.o gccDebug/time.o gccDebug/input.o gccDebug/script.o gccDebug/worldManager.o gccDebug/terrain.o gccDebug/fileManager.o gccDebug/sceneManager.o gccDebug/controlManager.o gccDebug/definitions.o gccDebug/ephemeris.o gccDebug/glew.o $(Debug_Library_Path) $(Debug_Libraries) -Wl,-rpath,./ -o gccDebug/FighterPilot.exe

# Compiles file game.cpp for the Debug configuration...
-include gccDebug/game.d
gccDebug/game.o: game.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c game.cpp $(Debug_Include_Path) -o gccDebug/game.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM game.cpp $(Debug_Include_Path) > gccDebug/game.d

# Compiles file campaign.cpp for the Debug configuration...
-include gccDebug/campaign.d
gccDebug/campaign.o: campaign.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c campaign.cpp $(Debug_Include_Path) -o gccDebug/campaign.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM campaign.cpp $(Debug_Include_Path) > gccDebug/campaign.d

# Compiles file levelEditor.cpp for the Debug configuration...
-include gccDebug/levelEditor.d
gccDebug/levelEditor.o: levelEditor.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c levelEditor.cpp $(Debug_Include_Path) -o gccDebug/levelEditor.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM levelEditor.cpp $(Debug_Include_Path) > gccDebug/levelEditor.d

# Compiles file menus.cpp for the Debug configuration...
-include gccDebug/menus.d
gccDebug/menus.o: menus.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c menus.cpp $(Debug_Include_Path) -o gccDebug/menus.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM menus.cpp $(Debug_Include_Path) > gccDebug/menus.d

# Compiles file dogFight.cpp for the Debug configuration...
-include gccDebug/dogFight.d
gccDebug/dogFight.o: dogFight.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c dogFight.cpp $(Debug_Include_Path) -o gccDebug/dogFight.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM dogFight.cpp $(Debug_Include_Path) > gccDebug/dogFight.d

# Compiles file splitScreen.cpp for the Debug configuration...
-include gccDebug/splitScreen.d
gccDebug/splitScreen.o: splitScreen.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c splitScreen.cpp $(Debug_Include_Path) -o gccDebug/splitScreen.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM splitScreen.cpp $(Debug_Include_Path) > gccDebug/splitScreen.d

# Compiles file blackSmoke.cpp for the Debug configuration...
-include gccDebug/blackSmoke.d
gccDebug/blackSmoke.o: blackSmoke.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c blackSmoke.cpp $(Debug_Include_Path) -o gccDebug/blackSmoke.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM blackSmoke.cpp $(Debug_Include_Path) > gccDebug/blackSmoke.d

# Compiles file bulletEffect.cpp for the Debug configuration...
-include gccDebug/bulletEffect.d
gccDebug/bulletEffect.o: bulletEffect.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bulletEffect.cpp $(Debug_Include_Path) -o gccDebug/bulletEffect.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bulletEffect.cpp $(Debug_Include_Path) > gccDebug/bulletEffect.d

# Compiles file contrail.cpp for the Debug configuration...
-include gccDebug/contrail.d
gccDebug/contrail.o: contrail.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c contrail.cpp $(Debug_Include_Path) -o gccDebug/contrail.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM contrail.cpp $(Debug_Include_Path) > gccDebug/contrail.d

# Compiles file contrailSmall.cpp for the Debug configuration...
-include gccDebug/contrailSmall.d
gccDebug/contrailSmall.o: contrailSmall.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c contrailSmall.cpp $(Debug_Include_Path) -o gccDebug/contrailSmall.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM contrailSmall.cpp $(Debug_Include_Path) > gccDebug/contrailSmall.d

# Compiles file debrisSmokeTrail.cpp for the Debug configuration...
-include gccDebug/debrisSmokeTrail.d
gccDebug/debrisSmokeTrail.o: debrisSmokeTrail.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c debrisSmokeTrail.cpp $(Debug_Include_Path) -o gccDebug/debrisSmokeTrail.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM debrisSmokeTrail.cpp $(Debug_Include_Path) > gccDebug/debrisSmokeTrail.d

# Compiles file explosionFlash.cpp for the Debug configuration...
-include gccDebug/explosionFlash.d
gccDebug/explosionFlash.o: explosionFlash.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c explosionFlash.cpp $(Debug_Include_Path) -o gccDebug/explosionFlash.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM explosionFlash.cpp $(Debug_Include_Path) > gccDebug/explosionFlash.d

# Compiles file explosionFlash2.cpp for the Debug configuration...
-include gccDebug/explosionFlash2.d
gccDebug/explosionFlash2.o: explosionFlash2.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c explosionFlash2.cpp $(Debug_Include_Path) -o gccDebug/explosionFlash2.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM explosionFlash2.cpp $(Debug_Include_Path) > gccDebug/explosionFlash2.d

# Compiles file explosionSmoke.cpp for the Debug configuration...
-include gccDebug/explosionSmoke.d
gccDebug/explosionSmoke.o: explosionSmoke.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c explosionSmoke.cpp $(Debug_Include_Path) -o gccDebug/explosionSmoke.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM explosionSmoke.cpp $(Debug_Include_Path) > gccDebug/explosionSmoke.d

# Compiles file explosionSparks.cpp for the Debug configuration...
-include gccDebug/explosionSparks.d
gccDebug/explosionSparks.o: explosionSparks.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c explosionSparks.cpp $(Debug_Include_Path) -o gccDebug/explosionSparks.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM explosionSparks.cpp $(Debug_Include_Path) > gccDebug/explosionSparks.d

# Compiles file groundExposionFlash.cpp for the Debug configuration...
-include gccDebug/groundExposionFlash.d
gccDebug/groundExposionFlash.o: groundExposionFlash.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c groundExposionFlash.cpp $(Debug_Include_Path) -o gccDebug/groundExposionFlash.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM groundExposionFlash.cpp $(Debug_Include_Path) > gccDebug/groundExposionFlash.d

# Compiles file particleExplosion.cpp for the Debug configuration...
-include gccDebug/particleExplosion.d
gccDebug/particleExplosion.o: particleExplosion.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c particleExplosion.cpp $(Debug_Include_Path) -o gccDebug/particleExplosion.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM particleExplosion.cpp $(Debug_Include_Path) > gccDebug/particleExplosion.d

# Compiles file planeContrail.cpp for the Debug configuration...
-include gccDebug/planeContrail.d
gccDebug/planeContrail.o: planeContrail.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c planeContrail.cpp $(Debug_Include_Path) -o gccDebug/planeContrail.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM planeContrail.cpp $(Debug_Include_Path) > gccDebug/planeContrail.d

# Compiles file planeEngines.cpp for the Debug configuration...
-include gccDebug/planeEngines.d
gccDebug/planeEngines.o: planeEngines.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c planeEngines.cpp $(Debug_Include_Path) -o gccDebug/planeEngines.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM planeEngines.cpp $(Debug_Include_Path) > gccDebug/planeEngines.d

# Compiles file smokeTrail.cpp for the Debug configuration...
-include gccDebug/smokeTrail.d
gccDebug/smokeTrail.o: smokeTrail.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c smokeTrail.cpp $(Debug_Include_Path) -o gccDebug/smokeTrail.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM smokeTrail.cpp $(Debug_Include_Path) > gccDebug/smokeTrail.d

# Compiles file splash.cpp for the Debug configuration...
-include gccDebug/splash.d
gccDebug/splash.o: splash.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c splash.cpp $(Debug_Include_Path) -o gccDebug/splash.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM splash.cpp $(Debug_Include_Path) > gccDebug/splash.d

# Compiles file splash2.cpp for the Debug configuration...
-include gccDebug/splash2.d
gccDebug/splash2.o: splash2.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c splash2.cpp $(Debug_Include_Path) -o gccDebug/splash2.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM splash2.cpp $(Debug_Include_Path) > gccDebug/splash2.d

# Compiles file plane.cpp for the Debug configuration...
-include gccDebug/plane.d
gccDebug/plane.o: plane.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c plane.cpp $(Debug_Include_Path) -o gccDebug/plane.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM plane.cpp $(Debug_Include_Path) > gccDebug/plane.d

# Compiles file aaGun.cpp for the Debug configuration...
-include gccDebug/aaGun.d
gccDebug/aaGun.o: aaGun.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c aaGun.cpp $(Debug_Include_Path) -o gccDebug/aaGun.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM aaGun.cpp $(Debug_Include_Path) > gccDebug/aaGun.d

# Compiles file bomb.cpp for the Debug configuration...
-include gccDebug/bomb.d
gccDebug/bomb.o: bomb.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bomb.cpp $(Debug_Include_Path) -o gccDebug/bomb.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bomb.cpp $(Debug_Include_Path) > gccDebug/bomb.d

# Compiles file missile.cpp for the Debug configuration...
-include gccDebug/missile.d
gccDebug/missile.o: missile.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c missile.cpp $(Debug_Include_Path) -o gccDebug/missile.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM missile.cpp $(Debug_Include_Path) > gccDebug/missile.d

# Compiles file bullet.cpp for the Debug configuration...
-include gccDebug/bullet.d
gccDebug/bullet.o: bullet.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c bullet.cpp $(Debug_Include_Path) -o gccDebug/bullet.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM bullet.cpp $(Debug_Include_Path) > gccDebug/bullet.d

# Compiles file player.cpp for the Debug configuration...
-include gccDebug/player.d
gccDebug/player.o: player.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c player.cpp $(Debug_Include_Path) -o gccDebug/player.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM player.cpp $(Debug_Include_Path) > gccDebug/player.d

# Compiles file settings.cpp for the Debug configuration...
-include gccDebug/settings.d
gccDebug/settings.o: settings.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c settings.cpp $(Debug_Include_Path) -o gccDebug/settings.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM settings.cpp $(Debug_Include_Path) > gccDebug/settings.d

# Compiles file level.cpp for the Debug configuration...
-include gccDebug/level.d
gccDebug/level.o: level.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c level.cpp $(Debug_Include_Path) -o gccDebug/level.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM level.cpp $(Debug_Include_Path) > gccDebug/level.d

# Compiles file objectInfo.cpp for the Debug configuration...
-include gccDebug/objectInfo.d
gccDebug/objectInfo.o: objectInfo.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c objectInfo.cpp $(Debug_Include_Path) -o gccDebug/objectInfo.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM objectInfo.cpp $(Debug_Include_Path) > gccDebug/objectInfo.d

# Compiles file debugBreak.cpp for the Debug configuration...
-include gccDebug/debugBreak.d
gccDebug/debugBreak.o: debugBreak.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c debugBreak.cpp $(Debug_Include_Path) -o gccDebug/debugBreak.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM debugBreak.cpp $(Debug_Include_Path) > gccDebug/debugBreak.d

# Compiles file engine.cpp for the Debug configuration...
-include gccDebug/engine.d
gccDebug/engine.o: engine.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c engine.cpp $(Debug_Include_Path) -o gccDebug/engine.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM engine.cpp $(Debug_Include_Path) > gccDebug/engine.d

# Compiles file graphicsManager.cpp for the Debug configuration...
-include gccDebug/graphicsManager.d
gccDebug/graphicsManager.o: graphicsManager.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c graphicsManager.cpp $(Debug_Include_Path) -o gccDebug/graphicsManager.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM graphicsManager.cpp $(Debug_Include_Path) > gccDebug/graphicsManager.d

# Compiles file openGLgraphics.cpp for the Debug configuration...
-include gccDebug/openGLgraphics.d
gccDebug/openGLgraphics.o: openGLgraphics.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c openGLgraphics.cpp $(Debug_Include_Path) -o gccDebug/openGLgraphics.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM openGLgraphics.cpp $(Debug_Include_Path) > gccDebug/openGLgraphics.d

# Compiles file collide.cpp for the Debug configuration...
-include gccDebug/collide.d
gccDebug/collide.o: collide.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c collide.cpp $(Debug_Include_Path) -o gccDebug/collide.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM collide.cpp $(Debug_Include_Path) > gccDebug/collide.d

# Compiles file gameMath.cpp for the Debug configuration...
-include gccDebug/gameMath.d
gccDebug/gameMath.o: gameMath.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c gameMath.cpp $(Debug_Include_Path) -o gccDebug/gameMath.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM gameMath.cpp $(Debug_Include_Path) > gccDebug/gameMath.d

# Compiles file angle.cpp for the Debug configuration...
-include gccDebug/angle.d
gccDebug/angle.o: angle.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c angle.cpp $(Debug_Include_Path) -o gccDebug/angle.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM angle.cpp $(Debug_Include_Path) > gccDebug/angle.d

# Compiles file dataManager.cpp for the Debug configuration...
-include gccDebug/dataManager.d
gccDebug/dataManager.o: dataManager.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c dataManager.cpp $(Debug_Include_Path) -o gccDebug/dataManager.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM dataManager.cpp $(Debug_Include_Path) > gccDebug/dataManager.d

# Compiles file assetLoader.cpp for the Debug configuration...
-include gccDebug/assetLoader.d
gccDebug/assetLoader.o: assetLoader.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c assetLoader.cpp $(Debug_Include_Path) -o gccDebug/assetLoader.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM assetLoader.cpp $(Debug_Include_Path) > gccDebug/assetLoader.d

# Compiles file object.cpp for the Debug configuration...
-include gccDebug/object.d
gccDebug/object.o: object.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c object.cpp $(Debug_Include_Path) -o gccDebug/object.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM object.cpp $(Debug_Include_Path) > gccDebug/object.d

# Compiles file objectList.cpp for the Debug configuration...
-include gccDebug/objectList.d
gccDebug/objectList.o: objectList.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c objectList.cpp $(Debug_Include_Path) -o gccDebug/objectList.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM objectList.cpp $(Debug_Include_Path) > gccDebug/objectList.d

# Compiles file menuScreen.cpp for the Debug configuration...
-include gccDebug/menuScreen.d
gccDebug/menuScreen.o: menuScreen.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c menuScreen.cpp $(Debug_Include_Path) -o gccDebug/menuScreen.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM menuScreen.cpp $(Debug_Include_Path) > gccDebug/menuScreen.d

# Compiles file particleManager.cpp for the Debug configuration...
-include gccDebug/particleManager.d
gccDebug/particleManager.o: particleManager.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c particleManager.cpp $(Debug_Include_Path) -o gccDebug/particleManager.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM particleManager.cpp $(Debug_Include_Path) > gccDebug/particleManager.d

# Compiles file random.cpp for the Debug configuration...
-include gccDebug/random.d
gccDebug/random.o: random.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c random.cpp $(Debug_Include_Path) -o gccDebug/random.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM random.cpp $(Debug_Include_Path) > gccDebug/random.d

# Compiles file time.cpp for the Debug configuration...
-include gccDebug/time.d
gccDebug/time.o: time.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c time.cpp $(Debug_Include_Path) -o gccDebug/time.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM time.cpp $(Debug_Include_Path) > gccDebug/time.d

# Compiles file input.cpp for the Debug configuration...
-include gccDebug/input.d
gccDebug/input.o: input.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c input.cpp $(Debug_Include_Path) -o gccDebug/input.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM input.cpp $(Debug_Include_Path) > gccDebug/input.d

# Compiles file script.cpp for the Debug configuration...
-include gccDebug/script.d
gccDebug/script.o: script.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c script.cpp $(Debug_Include_Path) -o gccDebug/script.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM script.cpp $(Debug_Include_Path) > gccDebug/script.d

# Compiles file worldManager.cpp for the Debug configuration...
-include gccDebug/worldManager.d
gccDebug/worldManager.o: worldManager.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c worldManager.cpp $(Debug_Include_Path) -o gccDebug/worldManager.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM worldManager.cpp $(Debug_Include_Path) > gccDebug/worldManager.d

# Compiles file terrain.cpp for the Debug configuration...
-include gccDebug/terrain.d
gccDebug/terrain.o: terrain.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c terrain.cpp $(Debug_Include_Path) -o gccDebug/terrain.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM terrain.cpp $(Debug_Include_Path) > gccDebug/terrain.d

# Compiles file fileManager.cpp for the Debug configuration...
-include gccDebug/fileManager.d
gccDebug/fileManager.o: fileManager.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c fileManager.cpp $(Debug_Include_Path) -o gccDebug/fileManager.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM fileManager.cpp $(Debug_Include_Path) > gccDebug/fileManager.d

# Compiles file sceneManager.cpp for the Debug configuration...
-include gccDebug/sceneManager.d
gccDebug/sceneManager.o: sceneManager.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c sceneManager.cpp $(Debug_Include_Path) -o gccDebug/sceneManager.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM sceneManager.cpp $(Debug_Include_Path) > gccDebug/sceneManager.d

# Compiles file controlManager.cpp for the Debug configuration...
-include gccDebug/controlManager.d
gccDebug/controlManager.o: controlManager.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c controlManager.cpp $(Debug_Include_Path) -o gccDebug/controlManager.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM controlManager.cpp $(Debug_Include_Path) > gccDebug/controlManager.d

# Compiles file definitions.cpp for the Debug configuration...
-include gccDebug/definitions.d
gccDebug/definitions.o: definitions.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c definitions.cpp $(Debug_Include_Path) -o gccDebug/definitions.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM definitions.cpp $(Debug_Include_Path) > gccDebug/definitions.d

# Compiles file ephemeris.cpp for the Debug configuration...
-include gccDebug/ephemeris.d
gccDebug/ephemeris.o: ephemeris.cpp
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c ephemeris.cpp $(Debug_Include_Path) -o gccDebug/ephemeris.o
	$(CPP_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM ephemeris.cpp $(Debug_Include_Path) > gccDebug/ephemeris.d

# Compiles file glew.c for the Debug configuration...
-include gccDebug/glew.d
gccDebug/glew.o: glew.c
	$(C_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -c glew.c $(Debug_Include_Path) -o gccDebug/glew.o
	$(C_COMPILER) $(Debug_Preprocessor_Definitions) $(Debug_Compiler_Flags) -MM glew.c $(Debug_Include_Path) > gccDebug/glew.d

# Builds the Release configuration...
.PHONY: Release
Release: create_folders gccRelease/game.o gccRelease/campaign.o gccRelease/levelEditor.o gccRelease/menus.o gccRelease/dogFight.o gccRelease/splitScreen.o gccRelease/blackSmoke.o gccRelease/bulletEffect.o gccRelease/contrail.o gccRelease/contrailSmall.o gccRelease/debrisSmokeTrail.o gccRelease/explosionFlash.o gccRelease/explosionFlash2.o gccRelease/explosionSmoke.o gccRelease/explosionSparks.o gccRelease/groundExposionFlash.o gccRelease/particleExplosion.o gccRelease/planeContrail.o gccRelease/planeEngines.o gccRelease/smokeTrail.o gccRelease/splash.o gccRelease/splash2.o gccRelease/plane.o gccRelease/aaGun.o gccRelease/bomb.o gccRelease/missile.o gccRelease/bullet.o gccRelease/player.o gccRelease/settings.o gccRelease/level.o gccRelease/objectInfo.o gccRelease/debugBreak.o gccRelease/engine.o gccRelease/graphicsManager.o gccRelease/openGLgraphics.o gccRelease/collide.o gccRelease/gameMath.o gccRelease/angle.o gccRelease/dataManager.o gccRelease/assetLoader.o gccRelease/object.o gccRelease/objectList.o gccRelease/menuScreen.o gccRelease/particleManager.o gccRelease/random.o gccRelease/time.o gccRelease/input.o gccRelease/script.o gccRelease/worldManager.o gccRelease/terrain.o gccRelease/fileManager.o gccRelease/sceneManager.o gccRelease/controlManager.o gccRelease/definitions.o gccRelease/ephemeris.o gccRelease/glew.o 
	g++ gccRelease/game.o gccRelease/campaign.o gccRelease/levelEditor.o gccRelease/menus.o gccRelease/dogFight.o gccRelease/splitScreen.o gccRelease/blackSmoke.o gccRelease/bulletEffect.o gccRelease/contrail.o gccRelease/contrailSmall.o gccRelease/debrisSmokeTrail.o gccRelease/explosionFlash.o gccRelease/explosionFlash2.o gccRelease/explosionSmoke.o gccRelease/explosionSparks.o gccRelease/groundExposionFlash.o gccRelease/particleExplosion.o gccRelease/planeContrail.o gccRelease/planeEngines.o gccRelease/smokeTrail.o gccRelease/splash.o gccRelease/splash2.o gccRelease/plane.o gccRelease/aaGun.o gccRelease/bomb.o gccRelease/missile.o gccRelease/bullet.o gccRelease/player.o gccRelease/settings.o gccRelease/level.o gccRelease/objectInfo.o gccRelease/debugBreak.o gccRelease/engine.o gccRelease/graphicsManager.o gccRelease/openGLgraphics.o gccRelease/collide.o gccRelease/gameMath.o gccRelease/angle.o gccRelease/dataManager.o gccRelease/assetLoader.o gccRelease/object.o gccRelease/objectList.o gccRelease/menuScreen.o gccRelease/particleManager.o gccRelease/random.o gccRelease/time.o gccRelease/input.o gccRelease/script.o gccRelease/worldManager.o gccRelease/terrain.o gccRelease/fileManager.o gccRelease/sceneManager.o gccRelease/controlManager.o gccRelease/definitions.o gccRelease/ephemeris.o gccRelease/glew.o $(Release_Library_Path) $(Release_Libraries) -Wl,-rpath,./ -o gccRelease/FighterPilot.exe

# Compiles file game.cpp for the Release configuration...
-include gccRelease/game.d
gccRelease/game.o: game.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c game.cpp $(Release_Include_Path) -o gccRelease/game.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM game.cpp $(Release_Include_Path) > gccRelease/game.d

# Compiles file campaign.cpp for the Release configuration...
-include gccRelease/campaign.d
gccRelease/campaign.o: campaign.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c campaign.cpp $(Release_Include_Path) -o gccRelease/campaign.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM campaign.cpp $(Release_Include_Path) > gccRelease/campaign.d

# Compiles file levelEditor.cpp for the Release configuration...
-include gccRelease/levelEditor.d
gccRelease/levelEditor.o: levelEditor.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c levelEditor.cpp $(Release_Include_Path) -o gccRelease/levelEditor.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM levelEditor.cpp $(Release_Include_Path) > gccRelease/levelEditor.d

# Compiles file menus.cpp for the Release configuration...
-include gccRelease/menus.d
gccRelease/menus.o: menus.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c menus.cpp $(Release_Include_Path) -o gccRelease/menus.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM menus.cpp $(Release_Include_Path) > gccRelease/menus.d

# Compiles file dogFight.cpp for the Release configuration...
-include gccRelease/dogFight.d
gccRelease/dogFight.o: dogFight.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c dogFight.cpp $(Release_Include_Path) -o gccRelease/dogFight.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM dogFight.cpp $(Release_Include_Path) > gccRelease/dogFight.d

# Compiles file splitScreen.cpp for the Release configuration...
-include gccRelease/splitScreen.d
gccRelease/splitScreen.o: splitScreen.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c splitScreen.cpp $(Release_Include_Path) -o gccRelease/splitScreen.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM splitScreen.cpp $(Release_Include_Path) > gccRelease/splitScreen.d

# Compiles file blackSmoke.cpp for the Release configuration...
-include gccRelease/blackSmoke.d
gccRelease/blackSmoke.o: blackSmoke.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c blackSmoke.cpp $(Release_Include_Path) -o gccRelease/blackSmoke.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM blackSmoke.cpp $(Release_Include_Path) > gccRelease/blackSmoke.d

# Compiles file bulletEffect.cpp for the Release configuration...
-include gccRelease/bulletEffect.d
gccRelease/bulletEffect.o: bulletEffect.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bulletEffect.cpp $(Release_Include_Path) -o gccRelease/bulletEffect.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bulletEffect.cpp $(Release_Include_Path) > gccRelease/bulletEffect.d

# Compiles file contrail.cpp for the Release configuration...
-include gccRelease/contrail.d
gccRelease/contrail.o: contrail.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c contrail.cpp $(Release_Include_Path) -o gccRelease/contrail.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM contrail.cpp $(Release_Include_Path) > gccRelease/contrail.d

# Compiles file contrailSmall.cpp for the Release configuration...
-include gccRelease/contrailSmall.d
gccRelease/contrailSmall.o: contrailSmall.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c contrailSmall.cpp $(Release_Include_Path) -o gccRelease/contrailSmall.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM contrailSmall.cpp $(Release_Include_Path) > gccRelease/contrailSmall.d

# Compiles file debrisSmokeTrail.cpp for the Release configuration...
-include gccRelease/debrisSmokeTrail.d
gccRelease/debrisSmokeTrail.o: debrisSmokeTrail.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c debrisSmokeTrail.cpp $(Release_Include_Path) -o gccRelease/debrisSmokeTrail.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM debrisSmokeTrail.cpp $(Release_Include_Path) > gccRelease/debrisSmokeTrail.d

# Compiles file explosionFlash.cpp for the Release configuration...
-include gccRelease/explosionFlash.d
gccRelease/explosionFlash.o: explosionFlash.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c explosionFlash.cpp $(Release_Include_Path) -o gccRelease/explosionFlash.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM explosionFlash.cpp $(Release_Include_Path) > gccRelease/explosionFlash.d

# Compiles file explosionFlash2.cpp for the Release configuration...
-include gccRelease/explosionFlash2.d
gccRelease/explosionFlash2.o: explosionFlash2.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c explosionFlash2.cpp $(Release_Include_Path) -o gccRelease/explosionFlash2.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM explosionFlash2.cpp $(Release_Include_Path) > gccRelease/explosionFlash2.d

# Compiles file explosionSmoke.cpp for the Release configuration...
-include gccRelease/explosionSmoke.d
gccRelease/explosionSmoke.o: explosionSmoke.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c explosionSmoke.cpp $(Release_Include_Path) -o gccRelease/explosionSmoke.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM explosionSmoke.cpp $(Release_Include_Path) > gccRelease/explosionSmoke.d

# Compiles file explosionSparks.cpp for the Release configuration...
-include gccRelease/explosionSparks.d
gccRelease/explosionSparks.o: explosionSparks.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c explosionSparks.cpp $(Release_Include_Path) -o gccRelease/explosionSparks.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM explosionSparks.cpp $(Release_Include_Path) > gccRelease/explosionSparks.d

# Compiles file groundExposionFlash.cpp for the Release configuration...
-include gccRelease/groundExposionFlash.d
gccRelease/groundExposionFlash.o: groundExposionFlash.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c groundExposionFlash.cpp $(Release_Include_Path) -o gccRelease/groundExposionFlash.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM groundExposionFlash.cpp $(Release_Include_Path) > gccRelease/groundExposionFlash.d

# Compiles file particleExplosion.cpp for the Release configuration...
-include gccRelease/particleExplosion.d
gccRelease/particleExplosion.o: particleExplosion.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c particleExplosion.cpp $(Release_Include_Path) -o gccRelease/particleExplosion.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM particleExplosion.cpp $(Release_Include_Path) > gccRelease/particleExplosion.d

# Compiles file planeContrail.cpp for the Release configuration...
-include gccRelease/planeContrail.d
gccRelease/planeContrail.o: planeContrail.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c planeContrail.cpp $(Release_Include_Path) -o gccRelease/planeContrail.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM planeContrail.cpp $(Release_Include_Path) > gccRelease/planeContrail.d

# Compiles file planeEngines.cpp for the Release configuration...
-include gccRelease/planeEngines.d
gccRelease/planeEngines.o: planeEngines.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c planeEngines.cpp $(Release_Include_Path) -o gccRelease/planeEngines.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM planeEngines.cpp $(Release_Include_Path) > gccRelease/planeEngines.d

# Compiles file smokeTrail.cpp for the Release configuration...
-include gccRelease/smokeTrail.d
gccRelease/smokeTrail.o: smokeTrail.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c smokeTrail.cpp $(Release_Include_Path) -o gccRelease/smokeTrail.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM smokeTrail.cpp $(Release_Include_Path) > gccRelease/smokeTrail.d

# Compiles file splash.cpp for the Release configuration...
-include gccRelease/splash.d
gccRelease/splash.o: splash.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c splash.cpp $(Release_Include_Path) -o gccRelease/splash.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM splash.cpp $(Release_Include_Path) > gccRelease/splash.d

# Compiles file splash2.cpp for the Release configuration...
-include gccRelease/splash2.d
gccRelease/splash2.o: splash2.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c splash2.cpp $(Release_Include_Path) -o gccRelease/splash2.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM splash2.cpp $(Release_Include_Path) > gccRelease/splash2.d

# Compiles file plane.cpp for the Release configuration...
-include gccRelease/plane.d
gccRelease/plane.o: plane.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c plane.cpp $(Release_Include_Path) -o gccRelease/plane.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM plane.cpp $(Release_Include_Path) > gccRelease/plane.d

# Compiles file aaGun.cpp for the Release configuration...
-include gccRelease/aaGun.d
gccRelease/aaGun.o: aaGun.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c aaGun.cpp $(Release_Include_Path) -o gccRelease/aaGun.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM aaGun.cpp $(Release_Include_Path) > gccRelease/aaGun.d

# Compiles file bomb.cpp for the Release configuration...
-include gccRelease/bomb.d
gccRelease/bomb.o: bomb.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bomb.cpp $(Release_Include_Path) -o gccRelease/bomb.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bomb.cpp $(Release_Include_Path) > gccRelease/bomb.d

# Compiles file missile.cpp for the Release configuration...
-include gccRelease/missile.d
gccRelease/missile.o: missile.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c missile.cpp $(Release_Include_Path) -o gccRelease/missile.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM missile.cpp $(Release_Include_Path) > gccRelease/missile.d

# Compiles file bullet.cpp for the Release configuration...
-include gccRelease/bullet.d
gccRelease/bullet.o: bullet.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c bullet.cpp $(Release_Include_Path) -o gccRelease/bullet.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM bullet.cpp $(Release_Include_Path) > gccRelease/bullet.d

# Compiles file player.cpp for the Release configuration...
-include gccRelease/player.d
gccRelease/player.o: player.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c player.cpp $(Release_Include_Path) -o gccRelease/player.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM player.cpp $(Release_Include_Path) > gccRelease/player.d

# Compiles file settings.cpp for the Release configuration...
-include gccRelease/settings.d
gccRelease/settings.o: settings.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c settings.cpp $(Release_Include_Path) -o gccRelease/settings.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM settings.cpp $(Release_Include_Path) > gccRelease/settings.d

# Compiles file level.cpp for the Release configuration...
-include gccRelease/level.d
gccRelease/level.o: level.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c level.cpp $(Release_Include_Path) -o gccRelease/level.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM level.cpp $(Release_Include_Path) > gccRelease/level.d

# Compiles file objectInfo.cpp for the Release configuration...
-include gccRelease/objectInfo.d
gccRelease/objectInfo.o: objectInfo.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c objectInfo.cpp $(Release_Include_Path) -o gccRelease/objectInfo.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM objectInfo.cpp $(Release_Include_Path) > gccRelease/objectInfo.d

# Compiles file debugBreak.cpp for the Release configuration...
-include gccRelease/debugBreak.d
gccRelease/debugBreak.o: debugBreak.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c debugBreak.cpp $(Release_Include_Path) -o gccRelease/debugBreak.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM debugBreak.cpp $(Release_Include_Path) > gccRelease/debugBreak.d

# Compiles file engine.cpp for the Release configuration...
-include gccRelease/engine.d
gccRelease/engine.o: engine.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c engine.cpp $(Release_Include_Path) -o gccRelease/engine.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM engine.cpp $(Release_Include_Path) > gccRelease/engine.d

# Compiles file graphicsManager.cpp for the Release configuration...
-include gccRelease/graphicsManager.d
gccRelease/graphicsManager.o: graphicsManager.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c graphicsManager.cpp $(Release_Include_Path) -o gccRelease/graphicsManager.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM graphicsManager.cpp $(Release_Include_Path) > gccRelease/graphicsManager.d

# Compiles file openGLgraphics.cpp for the Release configuration...
-include gccRelease/openGLgraphics.d
gccRelease/openGLgraphics.o: openGLgraphics.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c openGLgraphics.cpp $(Release_Include_Path) -o gccRelease/openGLgraphics.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM openGLgraphics.cpp $(Release_Include_Path) > gccRelease/openGLgraphics.d

# Compiles file collide.cpp for the Release configuration...
-include gccRelease/collide.d
gccRelease/collide.o: collide.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c collide.cpp $(Release_Include_Path) -o gccRelease/collide.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM collide.cpp $(Release_Include_Path) > gccRelease/collide.d

# Compiles file gameMath.cpp for the Release configuration...
-include gccRelease/gameMath.d
gccRelease/gameMath.o: gameMath.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c gameMath.cpp $(Release_Include_Path) -o gccRelease/gameMath.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM gameMath.cpp $(Release_Include_Path) > gccRelease/gameMath.d

# Compiles file angle.cpp for the Release configuration...
-include gccRelease/angle.d
gccRelease/angle.o: angle.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c angle.cpp $(Release_Include_Path) -o gccRelease/angle.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM angle.cpp $(Release_Include_Path) > gccRelease/angle.d

# Compiles file dataManager.cpp for the Release configuration...
-include gccRelease/dataManager.d
gccRelease/dataManager.o: dataManager.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c dataManager.cpp $(Release_Include_Path) -o gccRelease/dataManager.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM dataManager.cpp $(Release_Include_Path) > gccRelease/dataManager.d

# Compiles file assetLoader.cpp for the Release configuration...
-include gccRelease/assetLoader.d
gccRelease/assetLoader.o: assetLoader.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c assetLoader.cpp $(Release_Include_Path) -o gccRelease/assetLoader.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM assetLoader.cpp $(Release_Include_Path) > gccRelease/assetLoader.d

# Compiles file object.cpp for the Release configuration...
-include gccRelease/object.d
gccRelease/object.o: object.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c object.cpp $(Release_Include_Path) -o gccRelease/object.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM object.cpp $(Release_Include_Path) > gccRelease/object.d

# Compiles file objectList.cpp for the Release configuration...
-include gccRelease/objectList.d
gccRelease/objectList.o: objectList.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c objectList.cpp $(Release_Include_Path) -o gccRelease/objectList.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM objectList.cpp $(Release_Include_Path) > gccRelease/objectList.d

# Compiles file menuScreen.cpp for the Release configuration...
-include gccRelease/menuScreen.d
gccRelease/menuScreen.o: menuScreen.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c menuScreen.cpp $(Release_Include_Path) -o gccRelease/menuScreen.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM menuScreen.cpp $(Release_Include_Path) > gccRelease/menuScreen.d

# Compiles file particleManager.cpp for the Release configuration...
-include gccRelease/particleManager.d
gccRelease/particleManager.o: particleManager.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c particleManager.cpp $(Release_Include_Path) -o gccRelease/particleManager.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM particleManager.cpp $(Release_Include_Path) > gccRelease/particleManager.d

# Compiles file random.cpp for the Release configuration...
-include gccRelease/random.d
gccRelease/random.o: random.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c random.cpp $(Release_Include_Path) -o gccRelease/random.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM random.cpp $(Release_Include_Path) > gccRelease/random.d

# Compiles file time.cpp for the Release configuration...
-include gccRelease/time.d
gccRelease/time.o: time.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c time.cpp $(Release_Include_Path) -o gccRelease/time.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM time.cpp $(Release_Include_Path) > gccRelease/time.d

# Compiles file input.cpp for the Release configuration...
-include gccRelease/input.d
gccRelease/input.o: input.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c input.cpp $(Release_Include_Path) -o gccRelease/input.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM input.cpp $(Release_Include_Path) > gccRelease/input.d

# Compiles file script.cpp for the Release configuration...
-include gccRelease/script.d
gccRelease/script.o: script.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c script.cpp $(Release_Include_Path) -o gccRelease/script.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM script.cpp $(Release_Include_Path) > gccRelease/script.d

# Compiles file worldManager.cpp for the Release configuration...
-include gccRelease/worldManager.d
gccRelease/worldManager.o: worldManager.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c worldManager.cpp $(Release_Include_Path) -o gccRelease/worldManager.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM worldManager.cpp $(Release_Include_Path) > gccRelease/worldManager.d

# Compiles file terrain.cpp for the Release configuration...
-include gccRelease/terrain.d
gccRelease/terrain.o: terrain.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c terrain.cpp $(Release_Include_Path) -o gccRelease/terrain.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM terrain.cpp $(Release_Include_Path) > gccRelease/terrain.d

# Compiles file fileManager.cpp for the Release configuration...
-include gccRelease/fileManager.d
gccRelease/fileManager.o: fileManager.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c fileManager.cpp $(Release_Include_Path) -o gccRelease/fileManager.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM fileManager.cpp $(Release_Include_Path) > gccRelease/fileManager.d

# Compiles file sceneManager.cpp for the Release configuration...
-include gccRelease/sceneManager.d
gccRelease/sceneManager.o: sceneManager.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c sceneManager.cpp $(Release_Include_Path) -o gccRelease/sceneManager.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM sceneManager.cpp $(Release_Include_Path) > gccRelease/sceneManager.d

# Compiles file controlManager.cpp for the Release configuration...
-include gccRelease/controlManager.d
gccRelease/controlManager.o: controlManager.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c controlManager.cpp $(Release_Include_Path) -o gccRelease/controlManager.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM controlManager.cpp $(Release_Include_Path) > gccRelease/controlManager.d

# Compiles file definitions.cpp for the Release configuration...
-include gccRelease/definitions.d
gccRelease/definitions.o: definitions.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c definitions.cpp $(Release_Include_Path) -o gccRelease/definitions.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM definitions.cpp $(Release_Include_Path) > gccRelease/definitions.d

# Compiles file ephemeris.cpp for the Release configuration...
-include gccRelease/ephemeris.d
gccRelease/ephemeris.o: ephemeris.cpp
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c ephemeris.cpp $(Release_Include_Path) -o gccRelease/ephemeris.o
	$(CPP_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM ephemeris.cpp $(Release_Include_Path) > gccRelease/ephemeris.d

# Compiles file glew.c for the Release configuration...
-include gccRelease/glew.d
gccRelease/glew.o: glew.c
	$(C_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -c glew.c $(Release_Include_Path) -o gccRelease/glew.o
	$(C_COMPILER) $(Release_Preprocessor_Definitions) $(Release_Compiler_Flags) -MM glew.c $(Release_Include_Path) > gccRelease/glew.d

# Creates the intermediate and output folders for each configuration...
.PHONY: create_folders
create_folders:
	mkdir -p gccDebug
	mkdir -p gccRelease

# Cleans intermediate and output files (objects, libraries, executables)...
.PHONY: clean
clean:
	rm -f gccDebug/*.o
	rm -f gccDebug/*.d
	rm -f gccDebug/*.a
	rm -f gccDebug/*.so
	rm -f gccDebug/*.dll
	rm -f gccDebug/*.exe
	rm -f gccRelease/*.o
	rm -f gccRelease/*.d
	rm -f gccRelease/*.a
	rm -f gccRelease/*.so
	rm -f gccRelease/*.dll
	rm -f gccRelease/*.exe

