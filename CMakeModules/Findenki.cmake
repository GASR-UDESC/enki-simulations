SET(enki_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${enki_PATH}
)

FIND_PATH(enki_INCLUDE_DIR
	HINTS
	$ENV{enki_DIR}
	PATH_SUFFIXES include/enki include
  /usr/local/include/enki
  /usr/local/include/enki/
	PATHS ${enki_SEARCH_PATHS}
)

FIND_LIBRARY(enki_LIBRARY_TEMP
	NAMES enki
	HINTS
	$ENV{enki_DIR}
	PATH_SUFFIXES lib64 lib
	PATHS ${enki_SEARCH_PATHS}
)

SET(enkiviewer_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${enkiviewer_PATH}
)

FIND_PATH(enkiviewer_INCLUDE_DIR
	HINTS
	$ENV{enkiviewer_DIR}
	PATH_SUFFIXES include/enkiviewer include
  /usr/local/include/viewer
  /usr/local/include/viewer/
	PATHS ${enkiviewer_SEARCH_PATHS}
)

FIND_LIBRARY(enkiviewer_LIBRARY_TEMP
	NAMES enkiviewer
	HINTS
	$ENV{enkiviewer_DIR}
	PATH_SUFFIXES lib64 lib
	PATHS ${enkiviewer_SEARCH_PATHS}
)