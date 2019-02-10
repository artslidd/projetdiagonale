clear all

NB_STEPS = 18;
NB_STEPS_AMP = 6;
ORDER = 4;
NB_BIQUADS = ceil(ORDER/2);
FMIN = 87;
FMAX = 9334;

coeffs = ones(NB_BIQUADS*NB_STEPS_AMP*NB_STEPS, 6);

index = 1;

for amp=1:NB_STEPS_AMP
    
    for k=1:NB_STEPS

        fc = FMIN * (FMAX/FMIN) ^((k-1)/(NB_STEPS-1));
        
        [b,a] = ellip(ORDER, 0.1, double(amp)/NB_STEPS_AMP * 40, fc*2.0/44100.0, 'high');
        
        
        coeffs(index:index+NB_BIQUADS-1,:) = tf2sos(b,a);
        
        index = index + NB_BIQUADS;

    end
end

fid = fopen('highpass.dat','w');
fwrite(fid, coeffs', 'float32');
fclose(fid);