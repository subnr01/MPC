#!/bin/bash
id=1
i=0
x=`ls *.png`
	for a in $x ; do
	 	# let curNum=$(10000 + (100 * id) + i)
	 	# echo "Curnum is $curNum"
	 	# echo "id is $id"
	 	# echo "File being read is $a"
	 	# echo "I before is $i" 	
	 		
	 	if ((i==20))
	 		then
	 			let id=id+1
	 			let i=0
	 			echo "i is $i "
	 			echo "Incrementing id now"
	 	 	fi	
	 	 	
	 	let i1=100*id 		 
	 	 	let fileval=$((10000 + i1 + i))
	 	 	echo "file: $a  renamed: left_$fileval.png"
	 	 	mv $a "left_$fileval.png"

	 	let i=i+1 

	 	
 	
done
