% 1 -> 78
% 2 -> 465
% 3 -> 210
% 4 -> 354
% 5 -> 923
% 6 -> 872

close all
load('dataTP7.mat');

yoyoyo = getfield(dataTP,'data');
pre_pythagore = resample(yoyoyo,1,8); % Resamplisation

datalog = zeros(length(pre_pythagore),1);
i = 1;
%Pythagorisation
 for z = i:1:length(pre_pythagore)
        
     datalog(z) = sqrt((pre_pythagore(z,1)).^2+(pre_pythagore(z,2)).^2+(pre_pythagore(z,3)).^2);
              
 end

l = (0:(length(datalog)-1));


low_freq = 1;
high_freq = 2;
sample_rate = 25;
filter_order = 6;

[A,B,C,D] = butter(filter_order,(2.*[low_freq high_freq]/sample_rate));
d = designfilt('bandpassiir','FilterOrder',filter_order, ...
    'HalfPowerFrequency1',low_freq,'HalfPowerFrequency2',high_freq, ...
    'SampleRate',sample_rate);

sos = ss2sos(A,B,C,D);
fvt = fvtool(sos,d,'Fs',sample_rate);
legend(fvt,'butter','designfilt')


y1 = filter(d, datalog(:));
y2 = diff(y1);
%y3 = diff(y2);
l2 = (0:(length(y1)-1));
l3 = (0:(length(y2)-1));
%l4 = (0:(length(y3)-1));



figure(1)
%plot(l, datalog(:), '-g')
%hold on
plot(l2, y1, '-r')
hold on
plot(l3, y2, '-b')
%plot(l4, y3, '-y')
%hold off

% Step counter
count = 0;

for k = 2:1:length(y1)-1
    if y1(k-1) * y1(k) < 0
        count = count + 1;
    end
end

count = count / 2

% Derivative step counter
count_d = 0;

for k = 2:1:length(y2)-1
    if y2(k-1) * y2(k) < 0
        count_d = count_d + 1;
    end
end

count_d = count_d / 2

% Threshold step counter
count_t = 0; thresh = 0.02;
for k = 2:1:length(y2)-1
    if y2(k-1) * y2(k) < 0 && abs(y1(k)) > thresh
        count_t = count_t + 1;
        line([k k], [-0.2 0.2])
    end
end
count_t = count_t / 2

ref = dataTP.nbStepsRef