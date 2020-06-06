
#define GEN_SH          "#!/bin/bash\n"  \
    "for filename in `find . -type f -name \"*.dot\" | xargs`\n"  \
    "do\n" \
    "   echo `date +\"%%T.%%3N\"` gen $filename png \n" \
    "   dot -Tpng -o ${filename%%.*}.png $filename\n" \
    "done\n" 
