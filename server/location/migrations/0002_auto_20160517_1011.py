# -*- coding: utf-8 -*-
# Generated by Django 1.9.6 on 2016-05-17 10:11
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('location', '0001_initial'),
    ]

    operations = [
        migrations.AlterField(
            model_name='device',
            name='id',
            field=models.IntegerField(primary_key=True, serialize=False),
        ),
        migrations.AlterField(
            model_name='location',
            name='id',
            field=models.IntegerField(primary_key=True, serialize=False),
        ),
        migrations.AlterField(
            model_name='waypoint',
            name='id',
            field=models.IntegerField(primary_key=True, serialize=False),
        ),
    ]
