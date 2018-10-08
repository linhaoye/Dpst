create table tbl_member (
  `memberid` integer primary key not null autoincrement,
  `username` varchar(50),
  `password` varchar(32),
  `address1` varchar(128),
  `address2` varchar(128),
  `email1` varchar(80),
  `email2` varchar(80),
  `city` varchar(30),
  `pin` varchar(30),
  `ipaddr` varchar(15),
  `phone1` varchar(30)
);

create table tbl_friend (
  `memberid` integer,
  `freindid` integer,
  primary key ("memberid", "freindid")
);

CREATE VIEW "tbl_friend_list" AS 
SELECT A.username, C.ipaddr, C.username AS username_a 
FROM tbl_member A INNER JOIN tbl_friend B ON A.memberid=B.memberid 
INNER JOIN tbl_member C ON B.freindid=C.memberid;