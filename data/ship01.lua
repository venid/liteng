
frame = {}
n = 1;

for i = 1, 5 do
 for j = 1, 5 do
  frame[n] = { 1,  i, 1, j,  0, 0, 0 }
  n = n + 1;
 end
end

for i = 2, 4 do
 for j = 2, 4 do
  frame[n] = { 1,  i, 4, j,  0, 0, 0 }
  n = n + 1;
 end
end

frame[n] = { 2,  3, 4, 1,  3, 0, 0 }
n = n + 1;
frame[n] = { 2,  3, 4, 5,  0, 0, 0 }
n = n + 1;
frame[n] = { 1,  3, 3, 5,  0, 0, 0 }
n = n + 1;
frame[n] = { 1,  3, 2, 5,  0, 0, 0 }
n = n + 1;
frame[n] = { 1,  3, 3, 1,  0, 0, 0 }
n = n + 1;
frame[n] = { 1,  3, 2, 1,  0, 0, 0 }
n = n + 1;
