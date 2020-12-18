# brs-cpp

![Multi-Platform Build](https://github.com/AideTechBot/brs-cpp/workflows/Multi-Platform%20Build/badge.svg)

## Development

**How to build:**
```
cmake --build [ Build directory ]
```
**How to run tests:**
```
cd [ Build directory ]
ctest --verbose
```

## Download

Go to the side bar and navigate to the release you want to download. You can grab the release tagged `latest` if you want the most up to date changes, or you can go by version.


**Do not clone the repo then copy the file, it is not packed with the libraries required.**

## How to use

Here's a simple example of loading and reading different things from a BRS file:

```cpp

#define BRS_BRICKADIA_IMPLEMENTATION_H
#include "brs.hpp"

#include <iostream>
#include <filesystem>

int main()
{

	BRS::Reader reader(std::filesystem::current_path().string() + "\\test_save.brs");

	reader.readHeader1();
	
	if(reader.loadedHeader1()) {
		BRS::Header1 h1 = reader.getHeader1();
		std::cout << h1.author.name << " " << h1.description << " " << h1.map << std::endl;
	}

	reader.readHeader2();
	
	if(reader.loadedHeader2()) {
		BRS::Header2 h2 = reader.getHeader2();
		for(auto owner : h2.brickOwners) {
			std::cout << owner.name <<  std::endl;
		}
	}

	reader.readBricks();
	
	if(reader.loadedBricks()) {
		BRS::Bricks b = reader.getBricks();
		for(auto brick : b) {
			std::cout << brick.visibility <<  std::endl;
		}
	}
}

```

## License
```
brs-cpp is a one file header library for reading and writing to BRS save files.
Copyright (C) 2020 AideTechBot

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see https://www.gnu.org/licenses/.
```
