%% READ UART SERIAL
%Project: Read serial port and write it down in a .dat file 
%
%Author: Luan Pena
%Date: 09/18/2019
%
%Link (Serial Port - Matlab): https://www.mathworks.com/help/matlab/ref/serialport.html
%
%%
 %########################################################################
 % i -> number of iteration.
 % data -> array value of 512 samples (ADC value).
 % data -> receive 8-bits LSB and 8-bits MSB, therefore there are 256 actual
 % samples, because ADC12 input value is 12 bit resolution.
 %-----------------------------------------------------------------------
 % Default Serial reading configuration:
 % Boud Rate: 9600 sps
 % No-Parity | 8 bits
 % #####################################################################
 
 
s=serial('COM9')               % COM 9 port (default port UART communication - MSP430FR5969)
fopen(s);                  
% the serial port object needs to be created before the microcontroler send the data
% 
input('Do you want to proceed?');
% after the confirmation, the reading is going to start

 fid = fopen('ADC.dat', 'w');   % create ADC file .dat
 
 for i = 0:1:3
     disp('Start sampling loop');
     data = uint8(fread(s));            % Reads the value from the Serial Port
     fprintf(fid, '%x\n', data.');      % writes the value in the .dat file
                                        % transpose the value matrix
    
     %fwrite(fid, data);
     %save('Adcfile.txt', 'data');
     %pause(0.002);
     %data_show = fread(s)   % show the data
 end

% close file Object and SerialPort Object
fclose(fid);
fclose(s);
delete(s);
clear s;

disp('Done');
