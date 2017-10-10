#Author: persist
#Abstract: how docker runs

# Components

	>* dockerd
	>* containered
	>* containered-shim
	>* runc


# Framework

		Docker Engine
			|
		containered
			|
	+---------------+---------------+
	|		|		|
	shim	`	shim		shim
	runc		runc		runc
	container	container	container


# dockerd

	Docker deamon.


# containered

	A deamon to manage images and execute containers by invoking runtime components.


# containered-shim

	Connect runc and container: shim->runc->container. When runc exits, shim will replace init process to be the new parent of container.


# runc

	A lightweight tool to run containers without docker dependencies.
