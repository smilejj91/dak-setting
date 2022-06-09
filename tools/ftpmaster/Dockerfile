FROM nginx:latest

ADD default /etc/nginx/site-available/default
ADD nginx.conf /etc/nginx/nginx.conf

EXPOSE 80 

CMD ["nginx", "-g", "daemon off;"]
