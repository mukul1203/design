# Practice software design

Repo structure:

1. Each design project will be implemented inside a folder in repo root, the name of the folder being the project name prefixed by the sequence number, like 1_vending_machine.
2. Each project folder will internally contain a <project_name>.cpp and hpp, the entry point into the project for its clients. There will be a main.cpp which will consume the project APIs, and will typically implement some kind of terminal UI/logs to verify the implementation's functionalities. 
3. There will also be a components folder, which will then further contain folders for individual components implementation inside, like item_dispenser etc. The headers for these components will be in the include folder inside the components folder, with public and private headers. Public headers to be used by clients, and private headers for inclusion within the project implementation files.
4. Parallel to the projects, there will be a components folder same as above, for reusable components that can be useful across many projects.
5. Each project with have a CMakeLists.txt to build it. We will generate make files and then build.
6. Third party components outside the projects will mostly be included as source, but if not, then libraries.
