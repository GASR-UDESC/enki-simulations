setup:
	./scripts/setup.sh

update_enki:
	./scripts/update_enki.sh

compile:
	./scripts/compile.sh

run:
	./build/projects/$(run_file)/run.sh
