
print ("Hello lua")

function fun(x)
    local step = 1
    print("Coroutine starting step = "..step.." with x = "..x)
    -- ...
    -- Возврат управления, промежуточных результатов 
    -- и ожидание продолжения выполнения.
    x = coroutine.yield(step)
    step = step + 1
    print("Coroutine intermediate step = "..step.." with x = "..x)
    -- ...
    -- Возврат управления, промежуточных результатов 
    -- и ожидание продолжения выполнения.
    x = coroutine.yield(step)
    step = step + 1
    print("Coroutine finishing step = "..step.." with x = "..x)
    -- ...
    -- Завершение выполнения и возврат результатов
    return step
end
