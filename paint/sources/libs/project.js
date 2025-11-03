let project = new Project("libs");

project.add_include_dir("./");
project.add_cfiles("io_mesh.h");
project.add_cfiles("io_mesh.c");
project.add_tsfiles("./");

project.flatten();
return project;
