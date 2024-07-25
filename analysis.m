close all
clear all


num_bits = 200;

opts = detectImportOptions("gateway_ber.csv");
data = readtable("gateway_ber.csv",opts);



error_bits = table2array(data(:,1));
blocksize = table2array(data(:,2));

package_error = table2array(data(:,3));
tissue_length = table2array(data(:,4));

vec_tot_err_bits = {};
vec_tot_bits = {};
vec_tot_pe = {};
vec_tissue_length = {};

for I = 1:1:length(error_bits)
    
    if I == 1
        total_error_bits = error_bits(I);
        total_bits = blocksize(I);
        total_package_errors = package_error(I);
        vec_tissue_length = {tissue_length(I)};

    elseif I > 1
        if(tissue_length(I) == tissue_length(I-1))

            total_error_bits = total_error_bits + error_bits(I);
            total_bits = total_bits + blocksize(I);
            total_package_errors = total_package_errors + package_error(I);
        
        else
            vec_tot_err_bits = {vec_tot_err_bits{1:end},total_error_bits};
            vec_tot_bits = {vec_tot_bits{1:end},total_bits};
            vec_tot_pe = {vec_tot_pe{1:end},total_package_errors};
            vec_tissue_length = {vec_tissue_length{1:end},tissue_length(I)};

            total_error_bits = 0;
            total_bits = 0;
            total_package_errors = 0;
        

        
        end
        

    end
end

vec_tot_err_bits = {vec_tot_err_bits{1:end},total_error_bits};
vec_tot_bits = {vec_tot_bits{1:end},total_bits};
vec_tot_pe = {vec_tot_pe{1:end},total_package_errors};

mat_tissue_length = cell2mat(vec_tissue_length);
mat_tot_pe = cell2mat(vec_tot_pe);
mat_tot_error_bits = cell2mat(vec_tot_err_bits);
mat_tot_bits = cell2mat(vec_tot_bits);

mat_ber = mat_tot_error_bits ./ mat_tot_bits;

num_package = mat_tot_bits ./ num_bits;

mat_tot_pe = mat_tot_pe ./ num_package;

% figure 
% plot(mat_tissue_length,mat_tot_pe)
% xlabel('tissue thickness')
% ylabel('Package Error')
% 
% title('Package Error')


% figure
% plot(mat_tissue_length, mat_ber)
% xlabel('tissue thickness')
% ylabel('ber')
% 
% title('Bit Error Rate')

fontsize=20;%fontsize value
figure;% open a new plot canvas
color = {'#0072BD','#D95319','#EDB120','#7E2F8E','#77AC30','#4DBEEE','#A2142F'}; %list of colors from https://www.mathworks.com/help/matlab/creating_plots/specify-plot-colors.html
line_width=linspace(1,3,length(color));%specify the width for each line in the plot

 hold on
 plot(mat_tissue_length*(10^3),mat_ber*100,'Color',color{1},'LineWidth',line_width(length(color)-1+1));
    %to depict on the same canvas
 plot(mat_tissue_length*(10^3),mat_tot_pe*100,'Color',color{2},'LineWidth',line_width(length(color)-2+1))

legend_text=cell(1,2);

  legend_text{1}= 'Bit Error Rate' ;
  legend_text{2}= 'Package Error Rate' ;

legend({legend_text{:}},...
    'Location','southeast','NumColumns',2,'Interpreter',"latex",'FontSize',fontsize);% more properties on the legend at https://www.mathworks.com/help/matlab/ref/legend.html

%legend({'line$_{1}$','line$_{2}$','line$_{3}$','line$_{4}$','line$_{5}$','line$_{6}$','line$_{7}$'},...
%    'Location','northwest','NumColumns',2,'Interpreter',"latex",'FontSize',fontsize);% more properties on the legend at https://www.mathworks.com/help/matlab/ref/legend.html
grid on;
hold off %to close the canvas

% set(gca, 'XTick', [0:10:20]);
% x_ticks=cell(1,3);
% for m=1:3
%   x_ticks{m}=strcat("$t=",num2str((m-1)*10),"$");
% end
% 
% set(gca, 'XTickLabel', mat_tissue_length*(10^3), 'TickLabelInterpreter', 'latex');

%example to print arbitrary lines in the x direction
%set(gca, 'XTick', 0:M-1);


title('Error rates');
xlabel('Tissue thickness [mm]','Interpreter','latex');
ylabel('Error percentage [\%]','Interpreter','latex');
set(gca,'FontSize',fontsize);
