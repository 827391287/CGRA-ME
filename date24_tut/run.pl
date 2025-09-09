#list of designs for the Riken_test
@designs = ("conv_unroll2.dot", "Stencil_unroll2.dot", "fft_radix4.dot");
@results = ();

#walk through the designs
for ($i = 0; $i <= $#designs; $i++) {
    
    #walk through CGRAs with different dimensions
    for ($j = 3; $j <= 6; $j++) {

	print "WORKING ON: $designs[$i] targetting $j X $j CGRA.\n";
	
	#delete old log file, if any 
	system("rm $designs[$i]_$j.log");

	#run CGRA-ME
	system("cgrame -g $designs[$i] -c 3 -m ClusteredMapper --arch-opts \"rows=$j cols=$j\" > $designs[$i]_$j.log");

	#check the log file for mapping success
	open(F1, "<", "$designs[$i]_$j.log");
	$success = 0;
	while (<F1>) {
	    if (m/Mapping Success: 1/) {
		$success = 1;
	    }
	}
	close (F1);
	push(@results, "$designs[$i], $j X $j CGRA MAPPING SUCCESS: $success\n");
	
    }
}

print "RESULTS:\n";
for ($i = 0; $i <= $#results; $i++) {
    print $results[$i];
}