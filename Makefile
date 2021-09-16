--dependencies = client.c dispatcher.c dll_runner.c ds.c server.c logger.c protocol.c
--libraries = -lpthread -ldl
--build-dir = build
--main-file = main.c
--output-filename = main.out
--output-filepath = ${--build-dir}/${--output-filename}

--test-dir = tests
--test-filename = tester.c
--test-filepath = ${--test-dir}/${--test-filename}
--test-output-filename = tester.out
--test-output-filepath = ${--test-dir}/${--test-output-filename}
--test-temp-filename = temp-file
--test-temp-filepath = ${--test-dir}/${--test-temp-filename}

main: ${--main-file} ${--dependencies}
	@echo "Compiling..."
	@mkdir -p ${--build-dir}
	@gcc ${--main-file} ${--dependencies}  ${--libraries} -o ${--output-filepath}
	@echo "Compiled to \"${--output-filepath}\""

test: ${--test-filepath} ${--dependencies}
	@gcc ${--test-filepath} ${--dependencies}  ${--libraries} -o ${--test-output-filepath}
	@${--test-output-filepath} ${--test-temp-filename}
	@rm ${--test-output-filepath} ${--test-temp-filename}

clean: ${--build-dir}
	@rm ${--build-dir}/*