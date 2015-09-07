do
 local param =
  { name = "test_scene",
    { comp = "Scene::Render", module = "View", point = 2 },
    { comp = "Scene::Build", module = "Factory", point = 2}
  }
 local tmp = Unit(param);
 Message(0x00000031, 0, tmp);
end
