do
 local param =
  { name = "camera",
    { comp = "Camera::Render", module = "View", point = 2, fovx = 50.0, znear = 0.01, zfar = 200.0},
    { comp = "Camera::Flight", module = "Physics", point = 2},
    { comp = "Camera::Orbit", module = "Physics", point = 2},
    { comp = "Camera::Translate", module = "Physics", point = 3},
    { comp = "Camera::Control", module = "Control", point = 5},
    { comp = "Event::Key", module = "Control", point = 10, key = 118, fun = Load_data("event/cam/1.lua")},
   -- { comp = "Event::Key", module = "Control", point = 10, key = 61, fun = Load_data("event/cam/2.lua")},
    { comp = "Event::Mouse", module = "Control", point = 10, event = 0x01, fun = Load_data("event/cam/3.lua")},
    { comp = "Event::Mouse", module = "Control", point = 9, event = 0x08, fun = Load_data("event/cam/4.lua")},
    { comp = "Event::Mouse", module = "Control", point = 10, event = 0x02, fun = Load_data("event/cam/5.lua")}
  }
 local tmp = Unit(param);
 Message(0x00000030, 0, tmp);

 param =
  { name = "render_pipeline",
    { comp = "Render::Lists", module = "Physics", point = 10 },
    { comp = "Render::Update", module = "View", point = 3 },
  --  { comp = "Render::ListDraw", module = "View", point = 3 },
    { comp = "Render::Geometry", module = "View", point = 4, program = Load("frame.sh") },
  --  { comp = "Render::Lighting", module = "View", point = 5 },
  --  { comp = "Render::Compound", module = "View", point = 6 },
    { comp = "Render::EnvDraw", module = "View", point = 7, program = Load("env.sh") },
 --   { comp = "Render::IntDraw", module = "View", point = 8 }
  }
 tmp = Unit(param);
 Message(0x00000030, 0, tmp);

 param =
  { name = "dynamics_pipeline",
    { comp = "Scene::World", module = "Physics", point = 3 },
    { comp = "Dynamics::Scenes", module = "Physics", point = 4 }
  }
 tmp = Unit(param);
 Message(0x00000030, 0, tmp);
end
