do
 local param =
  { name = "camera",
    { comp = "Camera::Render", module = "View", point = 2, fovx = 50.0, znear = 0.01, zfar = 20.0},
    { comp = "Camera::Translate", module = "Physics", point = 2},
    { comp = "Camera::Control", module = "Control", point = 5},
    { comp = "Event::Key", module = "Control", point = 10, key = 45, fun = Load_data("event/cam/1.lua")},
    { comp = "Event::Key", module = "Control", point = 10, key = 61, fun = Load_data("event/cam/2.lua")},
    { comp = "Event::Mouse", module = "Control", point = 10, event = 0x01, fun = Load_data("event/cam/3.lua")},
    { comp = "Event::Mouse", module = "Control", point = 9, event = 0x08, fun = Load_data("event/cam/4.lua")}
  }
 local tmp = Unit(param);
 Message(0x00000030, 0, tmp);

 param =
  { name = "render_pipeline",
    { comp = "Render::Clear", module = "View", point = 1 },
    { comp = "Render::ListDraw", module = "View", point = 3 },
    { comp = "Render::MaterialDraw", module = "View", point = 6, program = Load("pass3.sh")}
  }
 tmp = Unit(param);
 Message(0x00000030, 0, tmp);

 param =
  { name = "dynamics_pipeline",
    { comp = "Dynamics::Scenes", module = "Physics", point = 3}
  }
 tmp = Unit(param);
 Message(0x00000030, 0, tmp);
end
