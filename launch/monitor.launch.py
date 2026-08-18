#!/usr/bin/env python3
"""
Launch file for ShipLoader Monitor QT application.
"""

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    monitor = Node(
        package='shiploader_monitor',
        executable='shiploader_monitor',
        name='shiploader_monitor',
        output='screen',
    )

    return LaunchDescription([
        monitor,
    ])
