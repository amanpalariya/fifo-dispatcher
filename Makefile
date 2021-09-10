--dependencies = client.c dispatcher.c dll_runner.c ds.c server.c
--build-dir = build
--main-file = main.c
--output-filename = main.out
--output-filepath = ${--build-dir}/${--output-filename}

main: main.c ${--dependencies}
	@echo "Compiling..."
	@mkdir -p ${--build-dir}
	@gcc ${--main-file} ${--dependencies}  -lpthread -o ${--output-filepath}
	@echo "Compiled to \"${--output-filepath}\""

clean: ${--build-dir}
	@rm ${--build-dir}/*