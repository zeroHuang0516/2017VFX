function [ hdrImg ] = hdr( listName,timeName )
imgListFile = importdata(listName);
[imgCount, ncols] = size(imgListFile);
imgCount = imgCount -1;
images = {};
for i = 2: imgCount+1
	imgFileName = strcat(imgListFile{1},imgListFile{i});
	images = [images; imread(imgFileName) ];
end

shutter_time = importdata(timeName);
shutter_time = shutter_time';


Zmin = 0;
Zmax = 255;
w = zeros(Zmax-Zmin+1,1);
for i = Zmin:Zmax
	if i<=(Zmin+Zmax)/2
		w(i+1) = i-Zmin;
	else
		w(i+1) = Zmax - i;
	end
end


l = 10;
N = 100;
P = imgCount;
[H, W, C] = size(images{1});

B = log(shutter_time);



Z = zeros(N,P,3);
for i =1:N
	x = randi(W);
	y = randi(H);
	for j = 1:P
		Z(i,j,:) = images{j}(y,x,:);
	end
end

[gR, leR] = gsolve(Z(:,:,1),B,l,w);
[gG, leG] = gsolve(Z(:,:,2),B,l,w);
[gB, leB] = gsolve(Z(:,:,3),B,l,w);
g = [gR, gG, gB];


% radiance map
hdrImg = zeros(H,W,C);
for k = 1:C
	gg = g(:,k);
	for x = 1:W
		for y = 1:H
			denum = 0;
			num = 0;
			for j = 1:P
				denum = denum + w(images{j}(y,x,k)+1);
				num = num + w(images{j}(y,x,k)+1)*(gg(images{j}(y,x,k)+1)-B(j));
			end
			hdrImg(y,x,k) = exp(num/denum);
		end
	end
end


final_image = tonemap(hdrImg);
%imwrite(final_image, './result/result_image.jpg');

%plot response curves
%figure;
%hold on;
%xlabel('RGB Intensity');
%ylabel('ln(E)+ln(t)');

%title('RGB Camera Response Curve');
%plot(gR,'r');
%plot(gG,'g');
%plot(gB,'b');
%legend('R Channel','G Channel','B Channel','Location', 'southeast');


end

