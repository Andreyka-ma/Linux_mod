cmd_/home/andrey/Module/shadow/modules.order := {   echo /home/andrey/Module/shadow/hello_kernel_mod.ko; :; } | awk '!x[$$0]++' - > /home/andrey/Module/shadow/modules.order
