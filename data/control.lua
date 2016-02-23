do

 local param =
  { name = "game",
    { comp = "Event::Key", module = "Control", point = 9, key = 27, fun = Load_data("event/game/exit.lua")},
    { comp = "Event::Key", module = "Control", point = 9, key = 190, fun = Load_data("event/game/2.lua")},
    { comp = "Event::Key", module = "Control", point = 9, key = 191, fun = Load_data("event/game/3.lua")}
  }
 local tmp = Unit(param);
 Message(0x00000030, 0, tmp);

end
