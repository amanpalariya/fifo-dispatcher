--dependencies = client.c dispatcher.c dll_runner.c ds.c server.c logger.c
--libraries = -lpthread -ldl
--build-dir = build
--main-file = main.c
--output-filename = main.out
--output-filepath = ${--build-dir}/${--output-filename}

main: main.c ${--dependencies}
	@echo "Compiling..."
	@mkdir -p ${--build-dir}
	@gcc ${--main-file} ${--dependencies}  ${--libraries} -o ${--output-filepath}
	@echo "Compiled to \"${--output-filepath}\""

clean: ${--build-dir}
	@rm ${--build-dir}/*