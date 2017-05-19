# Concatenate the time fields
for f in *.csv
	do
		echo "Processing $f file.."
		paste -d, <(cut -d$'\t' -f1,2 $f | sed 's/\t/ /g') <(cut -d\t -f3- $f) > "../processed/$f.cut"
	done