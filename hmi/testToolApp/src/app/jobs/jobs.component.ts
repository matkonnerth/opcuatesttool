import { Component, OnInit } from '@angular/core';
import { FinishedJob } from '../job';
import { JobsService } from '../jobs.service';

@Component({
  selector: 'app-jobs',
  templateUrl: './jobs.component.html',
  styleUrls: ['./jobs.component.css']
})
export class JobsComponent implements OnInit {

  jobs: FinishedJob[] = [];
  selectedJob?: FinishedJob;

  constructor(private jobService: JobsService) { }

  ngOnInit(): void {
    this.getJobs();
  }

  onSelect(job: FinishedJob): void {
    this.selectedJob = job;
  }

  newJob(job: FinishedJob): void {
    this.jobService.createJob(job.request);
  }

  getJobs(): void {
    this.jobService.getJobs()
      .subscribe(jobs => this.jobs = jobs);
  }

}





