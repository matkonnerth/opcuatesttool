import { Component, OnInit } from '@angular/core';
import { Job } from '../job';
import { JobsService } from '../jobs.service';

@Component({
  selector: 'app-jobs',
  templateUrl: './jobs.component.html',
  styleUrls: ['./jobs.component.css']
})
export class JobsComponent implements OnInit {

  jobs: Job[] = [];
  selectedJob?: Job;

  constructor(private jobService: JobsService) { }

  ngOnInit(): void {
    this.getJobs();
  }

  onSelect(job: Job): void {
    this.selectedJob = job;
  }

  getJobs(): void {
    this.jobService.getJobs()
      .subscribe(jobs => this.jobs = jobs);
  }

}





