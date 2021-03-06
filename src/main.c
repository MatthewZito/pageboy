#include "common.h"
#include "repl.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	if (argc < 2) {
		DIE("%s\n", "Must provide a database filename");
	}

	char* filename = argv[1];
	Table* table = db_open(filename);

	InputBuffer* ib = ib_alloc();

	while (1) {
		rprompt();
		ib_read(ib);

		if (ib->buffer[0] == '.') {
			switch(proc_meta_cmd(ib, table)) {
				case META_SUCCESS:
					continue;
				case META_E_UNRECOGNIZED_CMD:
					fprintf(
						stderr,
						"Unrecognized command '%s'\n",
						ib->buffer
					);
					continue;
			}
		}

		Statement statement;

		switch (prepare_statement(ib, &statement)) {
			case PREPARE_SUCCESS:
				break;
			case PREPARE_E_SYNTAX:
				fprintf(
					stderr,
					"%s\n",
					"Syntax error; cannot execute statement"
				);

				continue;
			case PREPARE_E_UNRECOGNIZED_STMT:
				fprintf(
					stderr,
					"Unrecognized keyword at start of '%s'\n",
					ib->buffer
				);

				continue;

			case PREPARE_E_MAX_CH:
				fprintf(
					stderr,
					"%s\n",
					"Max characters exceeded"
				);

				continue;
		}

		switch(exec_statement(&statement, table)) {
			case EXEC_SUCCESS:
				fprintf(
					stdout,
					"%s\n",
					"Executed statement"
				);

				break;

			case EXEC_E_TABLE_CAP:
				fprintf(
					stderr,
					"%s\n",
					"Table memory full"
				);

				break;
		}

	}

	return EXIT_SUCCESS;
}
