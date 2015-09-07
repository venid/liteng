
View =
 {
   width       = 800,      -- размер окна по ширене
   height      = 600,      -- размер окна по высоте
   pos_x       = 300,       -- позиция по x
   pos_y       = 20       -- позиция по y
 }

Factory =
 { path =
    { "data/",
      "data/shaders/",
      "data/materials/",
      "data/mesh/"
    },
   storage =
    { "data/event.zip" },
   save = "data/save/",
   data =
    { "pipeline.lua",
      "control.lua",
      "scene.lua"
    }
 }

main(module("View"));
thread(module("Physics"), module("Control"));
thread(module("Factory"));
