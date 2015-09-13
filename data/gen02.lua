
function body(x, y, z)
 rx = 2.0^2;
 ry = 1.5^2;
 rz = 2.0^2;

 if x > 0.0 
  then tmp = (x * x) / (rx * 3.0);
  else tmp = (x * x) / rx;
  end

 tmp = tmp + (y * y) / ry;

 if z > 0.0
  then tmp = tmp + (z * z) / (rz * 2.0);
  else tmp = tmp + (z * z) / (rz + (z * z) / 1.2);
  end

 return tmp;
end
