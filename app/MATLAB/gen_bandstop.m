clear all

NB_STEPS = 18;
NB_STEPS_AMP = 6;
NB_FILTERS = (NB_STEPS - 1) * NB_STEPS / 2;
ORDER = 4;
NB_BIQUADS = ceil(ORDER/2);
FMIN = 87;
FMAX = 9334;

coeffs = zeros(NB_BIQUADS*NB_STEPS_AMP*NB_FILTERS, 6);

index = 1;

for amp=1:NB_STEPS_AMP
    
    for ka=1:NB_STEPS

        for kb=ka+1:NB_STEPS

            fca = FMIN * (FMAX/FMIN) ^((ka-1)/(NB_STEPS-1));
            fcb = FMIN * (FMAX/FMIN) ^((kb-1)/(NB_STEPS-1));

            [b,a] = ellip(ORDER/2, 0.1, double(amp)/NB_STEPS_AMP * 40, [fca*2.0/44100.0, fcb*2.0/44100.0],'stop');
            
            
            coeffs(index:index+NB_BIQUADS-1,:) = tf2sos(b,a);
            
            index = index + NB_BIQUADS;

        end

    end
end

fid = fopen('bandstop.dat', 'w')
fwrite(fid, coeffs','float32');
fclose(fid);