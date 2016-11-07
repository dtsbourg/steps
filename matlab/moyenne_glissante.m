%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%                   Moving average
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


close all

total_error = 0;
total_errord = 0;
total_errort = 0;
l=0;

for ii = 1:15
    l = l + 1;
    load(['dataTP' num2str(ii) '.mat'])
    
    %load('dataTP4.mat');
    
    %%%%%%%%%%%%%%%%%%%%%%
    %
    %   Resamplification
    %
    %%%%%%%%%%%%%%%%%%%%%%
    
    data = getfield(dataTP,'data');
    pre_pythagore = resample(data,1,8);
    
    
    %%%%%%%%%%%%%%%%%%%%%%
    %
    %    Pythagorisation
    %
    %%%%%%%%%%%%%%%%%%%%%%
    
    datalog = zeros(length(pre_pythagore),1);
    a = 1;
    
    for z = a:1:length(pre_pythagore)
        
        datalog(z) = sqrt((pre_pythagore(z,1)).^2 + (pre_pythagore(z,2)).^2 ...
            + (pre_pythagore(z,3)).^2);
        
    end
    
    
    %%%%%%%%%%%%%%%%%%%%%%
    %
    %   Moving average
    %
    %%%%%%%%%%%%%%%%%%%%%%
    
    % We calculate the average every 4 data set
    
    larg_moy_gliss = 4;
    moy_local = 0;
    
    moy_gliss = zeros(1, floor(length(datalog)/larg_moy_gliss));
    l_moy = (0:(length(moy_gliss)-1));
    
    for a = 1:4:length(datalog)
        
        for b = a:a+larg_moy_gliss
            if b < length(datalog)
                moy_local =  moy_local + datalog(b);
            end
        end
        
        if a == 1 % new table 4x smaller
            moy_gliss(a) = moy_local/larg_moy_gliss;
        else
            moy_gliss((a-1)/larg_moy_gliss) = moy_local/larg_moy_gliss;
        end
        
        moy_local = 0;
        
    end
    
    
    %%%%%%%%%%%%%%%%%%%%%%
    %
    %   Moving average
    %
    %%%%%%%%%%%%%%%%%%%%%%
    
    y2 = diff(moy_gliss);
    
    fig1 = figure(1)
    fig1.Position =[0 0 1000 300];
    plot(datalog)
    grid on
    title('Initial Data')
    xlabel('Time')
    ylabel('Amplitude')
    xlim([1200; 1600])
    ylim([0.6; 2.1])
    
    fig2 = figure(2)
    fig2.Position =[0 0 1000 300];
    plot(l_moy, moy_gliss, '-r')
    grid on
    title('Moving Average')
    xlabel('Time')
    ylabel('Amplitude')
    xlim([300; 400])
    ylim([0.6; 2.1])
    
    %%%%%%%%%%%%%%%%%%%%%%
    %
    %   step counter
    %
    %%%%%%%%%%%%%%%%%%%%%%
    
    
    % Simple step counter
    
    count = 0;
    
    for k = 2:1:length(y2)-1
        if y2(k) < 0
            count = count + 1;
        end
    end
    
    count = count / 2;
    
    % Derivative step counter
    
    count_d = 0;
    
    for k = 2:1:length(y2)-1
        if y2(k-1) * y2(k) < 0
            count_d = count_d + 1;
        end
    end
    
    count_d = count_d / 2;
    % Threshold step counter
    
    count_t = 0; thresh = 0.05;
    
    for k = 2:1:length(y2)-1
        if y2(k-1) * y2(k) < 0 && abs(y2(k)) > thresh
            count_t = count_t + 1;
        end
    end
    
    count_t = count_t / 2;
    
    
    % Errors
    
    ref = dataTP.nbStepsRef;
    
    erreur_cout = abs(ref-count)/ref;
    erreur_cout_d = abs(ref-count_d)/ref;
    erreur_cout_t = abs(ref-count_t)/ref;
    
    if ii ~= 7
        total_error =  total_error + abs(erreur_cout);
        total_errord = total_errord + abs(erreur_cout_d);
        total_errort = total_errort + abs(erreur_cout_t);
    end
end

mean_error =  total_error/(l-1)
mean_errord = total_errord/(l-1)
mean_errort = total_errort/(l-1)
