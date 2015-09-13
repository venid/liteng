do
 local param =
  { name = "test_scene",
    { comp = "Scene::Render", module = "View", point = 2 },
    { comp = "Space::Body", module = "Factory", point = 2, fun = "gen03.lua"}
  }
 local tmp = Unit(param);
 Message(0x00000031, 0, tmp);
end
