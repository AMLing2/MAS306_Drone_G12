clc; clear; close all

%% Normal Distribution Plotting

dx = 0.01;
x = -4 : dx : 4;

lw = 2;

mu = 0;
sigma = 1;

%%%% Figure 1 %%%% 

% normal, x, mu, sigma
y1 = pdf('normal', x, -2, sigma);
y2 = pdf('normal', x,  0, sigma);
y3 = pdf('normal', x,  2, sigma);

fig1 = figure;
plot(x,y1, LineWidth=lw, Color='#41729f')
hold on
plot(x,y2, LineWidth=lw, Color='#d49425')
plot(x,y3, LineWidth=lw, Color='#819d59')
ylim([0 1])
legend('$\mu=-2$','$\mu=2$','$\mu=2$','Interpreter','latex')
% title('$f(x)$ with $\sigma = 1', 'Interpreter','latex')
ax = gca; 
ax.FontSize = 16; 
set(gca,'box','off')
legend boxoff


%%%% Figure 2 %%%% 
y4 = pdf('normal', x, mu, 0.5);
y5 = pdf('normal', x, mu,   1);
y6 = pdf('normal', x, mu,   2);

figure
plot(x,y4, LineWidth=lw, Color='#41729f')
hold on
plot(x,y5, LineWidth=lw, Color='#d49425')
plot(x,y6, LineWidth=lw, Color='#819d59')
ylim([0 1])
l = legend('$\sigma=0.5$','$\sigma=1$','$\sigma=2$','Interpreter','latex');
% title('$f(x)$ with $\mu = 0', 'Interpreter','latex')
ax = gca; 
ax.FontSize = 16;
set(gca,'box','off')
legend boxoff