
# Read fuses 
./avrdude -C ../avrdude.conf -c usbasp -p t85 -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

# Run AtTiny at 8mhz, set lfuse ti 0xE2
./avrdude -C ../avrdude.conf -c usbasp -p t85 -U lfuse:w:0xE2:m
