do
 local param =
  { name = "test_scene",
    { comp = "Scene::Render", module = "View", point = 2 },
    { comp = "Space::Body", module = "Factory", point = 2, fun = "gen03.lua"}
  }
 local tmp = Unit(param);
 Message(0x00000031, 0, tmp);

 param =
  {  name = "part",
    { comp = "Scene::Render", module = "View", point = 2 },
    { comp = "Scene::Behavior", module = "Physics", point = 2 },
    { comp = "Scene::Build", module = "Factory", point = 2 },
    { comp = "Scene::Control", module = "Control", point = 5 }
  }
 tmp = Unit(param);
 Message(0x00000031, 0, tmp);

 --param =
  --{  name = "lost",
    --{ comp = "Scene::Render", module = "View", point = 2 },
    --{ comp = "Scene::Behavior", module = "Physics", point = 2 },
    --{ comp = "Scene::Build", module = "Factory", point = 2 },
    --{ comp = "Scene::Control", module = "Control", point = 5 }
  --}
 --tmp = Unit(param);
 --Message(0x00000031, 0, tmp);

 param =
  {  name = "structure",
    { comp = "Scene::Render", module = "View", point = 2 },
    { comp = "Ship::Make", module = "Factory", point = 2, blueprint = "ship01.lua" }
  }
 tmp = Unit(param);
 Message(0x00000031, 0, tmp);
end
