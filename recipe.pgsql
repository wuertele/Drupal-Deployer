CREATE TABLE recipe (
   nid integer not null default '0',
   source varchar(255),
   yield varchar(255),
   preptime integer DEFAULT '0',
   notes text,
   PRIMARY KEY (nid)
);

CREATE TABLE recipe_ingredients (
   iid SERIAL,   
   nid integer not null default '0',
   ingredient varchar(255),
   weight smallint DEFAULT '0',
   PRIMARY KEY (iid)
);
