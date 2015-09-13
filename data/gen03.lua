
function subbody(x, y, z)
 r = 1.0^2;
 
 local tmp = (x * x) / (r + (x * x));
 
 tmp = tmp + (y * y) / r;

 if z > 0.0
  then tmp = tmp + (z * z) / (r + (z * z));
  else tmp = tmp + (z * z) / r;
  end

 return tmp;
end

function body(x, y, z)
 rx = 2.0^2;
 ry = 1.5^2;
 rz = 2.0^2;

 local tmp = (x * x) / (rx * 3.0);
 
 if y > 0.0
  then tmp = tmp + (y * y) / (y * 3.0);
  else 
   tmp = tmp + (y * y) / ry;
  end


 --if z > 0.0
  --then tmp = tmp + (z * z) / (rz * 2.0);
  --else 
   tmp = tmp + (z * z) / (rz + (z * z) / 1.2);
  --end

 --if (tmp < 1.0) and (subbody(x, y, z) < 1.0)
   --then tmp = tmp + 1.0;
   --end
 
 return tmp;
end

noise =
{ symmetry = 0x00,
  rnd = 12,
  {octaves = 2, frequency = 0.3, amplitude = 3.0, pressure = 1.4},
  {octaves = 3, frequency = 0.8, amplitude = 1.0, pressure = 6.0}
}
