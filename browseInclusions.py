from os import walk, system

HEADERS = ["common.h",
		   "vectors/vectors.h",
		   "targetSpecific/macros.h", "targetSpecific/types.h",
		   "matrices/tracking.h", "matrices/matrix.h", "matrices/localCommon.h", "matrices/advMatrix.h", "matrices/gauss.h",
		   "memory/memfuncs.h",
		   "lists/list.h",
		   "linkedLists/linkedLists.h",
		   "endianness/endianness.h",
		   "dicts/dict.h",
		   "bools/bool.h",
		   "bits/bits.h"]

INCLUSIONS: dict[str: set[str]] = {}

system("clear")

for header in HEADERS:
	fp = open(header, "r")

	lines: list[str] = fp.read().split("\n")
	inclusions: list[str] = []

	INCLUSIONS[header] = set()

	for line in lines:
		if line.startswith("#include") and line.count("\"") == 2:
			include_path = line.split(" ")[1]

			if not "/" in include_path:
				include_path = "/".join(header.split("/")[0:-1] + [include_path])

			include_path = include_path.replace("\"", "").replace("../", "")

			#print(header, include_path)

			INCLUSIONS[header].add(include_path)

	fp.close()

#print(INCLUSIONS)

def dependencies(header: str) -> str:
	current_header: str = str(header)

	next_headers: set = set()
	explored_headers: set = set()

	to_add: set = set()

	while len(next_headers) >= 0:
		#print(current_header)
		to_add.clear()
		to_add.update(INCLUSIONS[current_header])

		to_add = to_add.difference(explored_headers)

		next_headers.update(to_add)

		explored_headers.add(current_header)

		if (len(next_headers) > 0):
			current_header = next_headers.pop()
		else:
			break

	output = ""

	for element in explored_headers:
		output += element + "\n"

	return output

print(dependencies("lists/list.h"))