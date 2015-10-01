
frame = {}
n = 1;

for i = 1, 5 do
 for j = 1, 5 do
  frame[n] = { 1,  i, 1, j,  0, 0, 0 }
  n = n + 1;
 end
end

for i = 1, 5 do
 for j = 2, 4 do
  frame[n] = { 1,  i, 4, j,  0, 0, 0 }
  n = n + 1;
 end
end

for i = 2, 3 do
 for j = 1, 5 do
  frame[n] = { 1,  j, i, 1,  0, 0, 0 }
  n = n + 1;
 end
end

for i = 2, 3 do
 for j = 1, 5 do
  frame[n] = { 1,  j, i, 5,  0, 0, 0 }
  n = n + 1;
 end
end

for i = 1, 5 do
 frame[n] = { 2,  i, 4, 5,  0, 0, 0 }
 n = n + 1;
end

for i = 1, 5 do
 frame[n] = { 2,  i, 4, 1,  0, 2, 0 }
 n = n + 1;
end

for i = 2, 3 do
 for j = 2, 4 do
  frame[n] = { 1,  1, i, j,  0, 0, 0 }
  n = n + 1;
 end
end

--frame[n] = { 2,  3, 4, 1,  1, 0, 0 }
--n = n + 1;
--frame[n] = { 2,  3, 4, 5,  0, 0, 0 }
--n = n + 1;
--frame[n] = { 1,  3, 3, 5,  0, 0, 0 }
--n = n + 1;
--frame[n] = { 1,  3, 2, 5,  0, 0, 0 }
--n = n + 1;
--frame[n] = { 1,  3, 3, 1,  0, 0, 0 }
--n = n + 1;
--frame[n] = { 1,  1, 1, 2,  0, 0, 0 }
--n = n + 1;
--frame[n] = { 2,  2, 1, 2,  0, 0, 0 }
--n = n + 1;
--frame[n] = { 3,  3, 1, 2,  0, 0, 0 }
--n = n + 1;
--frame[n] = { 3,  3, 1, 1,  0, 1, 0 }
--n = n + 1;
--frame[n] = { 2,  2, 1, 1,  0, 2, 0 }
--n = n + 1;
--frame[n] = { 6,  1, 1, 11,  0, 0, 3 }
--n = n + 1;
