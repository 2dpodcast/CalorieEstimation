% clear;
% clc;
% % dir=uigetdir('select the data sets...');  % ѡ���ļ���    
% save_path='F:\\ECUSTFDD\\';
% d=dir('F:\\ʳ��+���+����\\');
% di=struct2cell(d);
% for i=24:length(di)
%     if(length(di{1,i})<3)
%         continue;
%     else
%         if(di{4,i}==0)
%             continue;
%         end
%     end
%     path='F:\\ʳ��+���+����\\';
%     class=di{1,i};%ʳ��
%     if ~exist(strcat(save_path,class),'dir') 
%         mkdir(strcat(save_path,class))         % �������ڣ��ڵ�ǰĿ¼�в���һ����Ŀ¼
%     else
%         continue
%     end
%     
%     
%     path=strcat(path,class);
%     path=strcat(path,'\\');%F\ecust\apple\
%     img_path=dir(path);
%     img_path_=struct2cell(img_path);
%     for j=1:length(img_path_(1,:))
%         if(length(img_path_{1,j})<3)
%             continue;
%         end
%         file=strcat(path,img_path_{1,j});
%         file=strcat(file,'\\');
%         str = dir(strcat(file,'*.jp*')); % �滻�����Լ����ļ�����Ŀ¼
%         strx = struct2cell(str);
%         sn = length(strx(1,:));
%          for ix = 1:sn
%              newname=sprintf('%s\\\\%s-%d.jpg',strcat(save_path,class),img_path_{1,j},ix);
%              img=imread(strcat(file,strx{1,ix}));
%              [a,b,~]=size(img);
%              while(a>2000||b>2000)
%                  img=imresize(img,0.5);
%                  [a,b,~]=size(img);
%              end
%              imwrite(img,newname);
% %              movefile(strcat(path,strx{1,ix}),newname);
%          end
%      fprintf('%d/%d is finished\n',j,length(di));
%     end
%     
%     str = dir(strcat(path,'*.jp*')); % �滻�����Լ����ļ�����Ŀ¼
%     strx = struct2cell(str);
%      sn = length(strx(1,:));
%      for ix = 1:sn
%          newname=sprintf('%s%s%d.jpg',path,class,ix);
%          if(strcmp(strcat(path,strx{1,ix}),newname))%�Ѿ�����Ҫ������ֲ���Ҫ�޸�
%              continue;
%          end
%          movefile(strcat(path,strx{1,ix}),newname);
%      end
%      fprintf('%d/%d is finished\n',i,length(di));
%     
% end



clear;
clc;
% dir=uigetdir('select the data sets...');  % ѡ���ļ���    
d=dir('E:\\ECUSTFDD\\');
di=struct2cell(d);
for i=1:length(di)
    if(length(di{1,i})<3)
        continue;
    end
    path='E:\\ECUSTFDD\\';
    class=di{1,i};
    path=strcat(path,class);
    path=strcat(path,'\\');
    str = dir(strcat(path,'*.jp*')); % �滻�����Լ����ļ�����Ŀ¼
    strx = struct2cell(str);
     sn = length(strx(1,:));
     for ix = 1:sn
          img=imread(strcat(path,strx{1,ix}));
         [a,b,~]=size(img);
         while(a>1000||b>1000)
             img=imresize(img,0.5);
             [a,b,~]=size(img);
         end
         imwrite(img,strcat(path,strx{1,ix}));
     end
     fprintf('%d/%d is finished\n',i,length(di));
    
end

