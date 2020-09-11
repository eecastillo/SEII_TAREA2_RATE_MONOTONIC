# SEII_TAREA2_RATE_MONOTONIC
Se tomo como base el algoritmo visto en clase llamado contextSwitchFull, lo que se hizo fue agregar un campo a la estructura que se llamaba priority.
Asi mismo se agrego una enumeracion en la cual se encuentran las diferentes prioridades que puede existir, de 0 a 4. Se asignan las prioridades a cada una de las tareas,
tambien se agrego el tick a cada una de las tareas en la funcion delay.
Despues en la parte del kernel, tomamos como base el algortimo ya existente pero lo modificamos para poder buscar la prioridad mas alta. Esto se logro haciendo un recorrido
de toda la lista de tasks y se comparaba con la prioridad mas alta. Si se encontraba se revisaba su estado y en caso de ser necesario se ejecutaba. Si no se tenia que ejecutar,
se pasaba a la siguiente prioridad y asi hasta encontrar una task que ejecutar. 
A la tarea de idle se le asigno la prioridad mas baja, que corresponde a la prioridad 4. Por lo tanto si las otras tres tareas se estaban ejecutando, se llamada a la tarea idle.
Por ultimo para poder tener una mejor idea de que era lo que se estaba ejecutando se puso un campo en la estructura llamado id, que corresponde al numero de tarea que es cada una.
